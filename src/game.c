#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gb/gb.h>
#include <gbdk/emu_debug.h>
#include "bank_switch.h"
#include "board_manager.h"
#include "gamevars.h"
#include "game.h"
#include "elements.h"
#include "himem.h"
#include "input.h"
#include "math.h"
#include "../res/message_consts.h"
#include "oop.h"
#include "renderer.h"
#include "renderer_sidebar.h"
#include "sound_consts.h"
#include "timer.h"

const zoo_stat_t stat_template_default = {
	0, 0,
	0, 0, 0,
	0, 0, 0,
	STAT_ID_NONE, STAT_ID_NONE,
	{0, 0},
	0xFFFF, 0
};

AT(0x0068) const int8_t neighbor_delta_x[4] = {0, 0, -1, 1};
AT(0x006C) const int8_t neighbor_delta_y[4] = {-1, 1, 0, 0};
AT(0x0070) const int8_t diagonal_delta_x[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
AT(0x0078) const int8_t diagonal_delta_y[8] = {1, 1, 1, 0, -1, -1, -1, 0};

int8_t viewport_x = 1;
int8_t viewport_y = 1;

void center_viewport_on_player(void) {
	int8_t vx = ZOO_STAT(0).x - VIEWPORT_CENTER_X;
	int8_t vy = ZOO_STAT(0).y - VIEWPORT_CENTER_Y;
	if (vx < VIEWPORT_MIN_X) vx = VIEWPORT_MIN_X;
	else if (vx > VIEWPORT_MAX_X) vx = VIEWPORT_MAX_X;
	if (vy < VIEWPORT_MIN_Y) vy = VIEWPORT_MIN_Y;
	else if (vy > VIEWPORT_MAX_Y) vy = VIEWPORT_MAX_Y;

	viewport_x = vx;
	viewport_y = vy;
}

void board_enter_stage1(void) {
	zoo_board_info.start_player_x = ZOO_STAT(0).x;
	zoo_board_info.start_player_y = ZOO_STAT(0).y;

	zoo_world_info.board_time_sec = 0;
	zoo_game_state.game_state_element = E_PLAYER; // TODO: not here...

	center_viewport_on_player();
	text_update();
}

void board_enter_stage2(void) {
	sidebar_hide_message();
	game_update_sidebar_all();
}

void board_enter_stage3(void) {
	if ((zoo_board_info.flags & BOARD_IS_DARK) && !(msg_flags.f1 & MSG_FLAG1_HINT_TORCH)) {
		display_message(200, msg_torch_hint_line1, msg_torch_hint_line2, msg_torch_hint_line3);
		msg_flags.f1 |= MSG_FLAG1_HINT_TORCH;
	}
}

void board_change(uint8_t id) {
	zoo_tile_t *tile = &ZOO_TILE(ZOO_STAT(0).x, ZOO_STAT(0).y);
	tile->element = E_PLAYER;
	tile->color = 0x1F; // player color

	save_board(zoo_world_info.current_board);
	load_board(id);
	zoo_world_info.current_board = id;
	save_world(zoo_game_state.world_id);
}

void board_draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	// Viewport check
	uint8_t vx = x - viewport_x;
	if (vx >= VIEWPORT_WIDTH) return;
	uint8_t vy = y - viewport_y;
	if (vy >= VIEWPORT_HEIGHT) return;

	text_draw(vx, vy, chr, col);
}

void board_undraw_tile(uint8_t x, uint8_t y) {
	uint8_t vx = x - viewport_x;
	if (vx >= VIEWPORT_WIDTH) return;
	uint8_t vy = y - viewport_y;
	if (vy >= VIEWPORT_HEIGHT) return;

	text_undraw(vx, vy);
}

void board_draw_tile(uint8_t x, uint8_t y) {
	// Viewport check
	uint8_t vx = x - viewport_x;
	if (vx >= VIEWPORT_WIDTH) return;
	uint8_t vy = y - viewport_y;
	if (vy >= VIEWPORT_HEIGHT) return;

	zoo_tile_t tile;
	ZOO_TILE_ASSIGN(tile, x, y);

	// Darkness check
	if ((zoo_board_info.flags & BOARD_IS_DARK)) {
		if (!(zoo_element_defs_flags[tile.element] & ELEMENT_VISIBLE_IN_DARK)) {
			if (zoo_world_info.torch_ticks > 0) {
				int8_t dx = ZOO_STAT(0).x - x;
				int8_t dy = ZOO_STAT(0).y - y;
				int16_t dist = (int16_t)(dx*dx) + ((int16_t)(dy*dy) << 1);
				if (dist < TORCH_DIST_SQR) {
					goto NotDark;
				}
			}
			text_draw(vx, vy, 176, 0x07);
			return;
		}
	}

NotDark:
	if (tile.element == E_EMPTY) {
		text_draw(vx, vy, ' ', 0x0F);
	} else {
		if (tile.element < E_TEXT_MIN) {
			uint8_t ch;
			uint8_t prev_bank = _current_bank;
			ZOO_SWITCH_ROM(1);

			if (ZOO_ELEMENT_BOUNDS(tile.element) && zoo_element_defs_drawprocs[tile.element] != 0) {
				ch = zoo_element_defs_drawprocs[tile.element](x, y);
			} else {
				ch = zoo_element_defs_character[tile.element];
			}

			ZOO_SWITCH_ROM(prev_bank);

			text_draw(vx, vy, ch, tile.color);
		} else {
			uint8_t color;
			if (tile.element == E_TEXT_WHITE) {
				color = 0x0F;
			} else {
				color = 0x0F | ((tile.element - E_TEXT_MIN + 1) << 4);
			}
			text_draw(vx, vy, tile.color, color);
		}
	}
}

void board_redraw(void) {
	renderer_scrolling = 1;

	for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
		text_free_line(iy);
		for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
			board_draw_tile(ix + viewport_x, iy + viewport_y);
		}
	}

	renderer_scrolling = 0;
}

void init_display_message(uint8_t time, bool visible) {
	uint8_t sid = get_stat_id_at(0, 0);
	uint8_t dur;
	if (sid != STAT_ID_NONE) {
#ifdef BUGFIX_DIEMOVE_MESSAGE
		if (visible) {
			ZOO_TILE(0, 0).element = E_MESSAGE_TIMER;
			ZOO_STAT(sid).cycle = 1;
			goto SetDuration;
		}
#endif
		remove_stat(sid);
		sidebar_hide_message();
	}

	if (visible) {
		add_stat(0, 0, E_MESSAGE_TIMER, 0, 1, &stat_template_default);
		sid = zoo_stat_count;
SetDuration:
		dur = time / (zoo_game_state.tick_time_duration + 1);
		ZOO_STAT(sid).p2 = dur;
		sidebar_set_message_color(0x9 + (dur % 7));
	}
}

void display_message(uint8_t time, const char* line1, const char* line2, const char* line3) {
	init_display_message(time, line3 != NULL);
	if (line3 != NULL) {
		sidebar_show_message(line1, 3, line2, 3, line3, 3);
	}
}

uint8_t get_stat_id_at(uint8_t x, uint8_t y) {
#if defined(SM83)
	x; y;
__asm
	; TODO (SDCC 4.2.0 upgrade): optimize?
	ld		c, a
	; (hl) - stat pointer
	; a - compared value
	; ec (high, low) - checked value
	; d - max stat count
	; b - counter
	ldh		a, (_zoo_stat_count)
	inc		a
	ld		d, a
	ld		b, #0
	ld		hl, #(_zoo_stats+16)
GetStatIdAtLoop:
	ld		a, (hl+)
	cp		a, c
	ld		a, (hl+)
	jr		nz, GetStatIdAtNotFound
	cp		a, e
	jr		nz, GetStatIdAtNotFound
	ld		a, b
	ret
GetStatIdAtNotFound:
	; hl += 14
	ld		a, l
	add		a, #14
	ld		l, a
	adc		a, h
	sub		a, l
        ld              h, a
	; d -= 1
	inc		b
	dec		d
	jr		nz, GetStatIdAtLoop
GetStatIdFinished:
	ld      	a, #0xff
__endasm;
#else
	zoo_stat_t *stat = zoo_stats + 1;

	for (uint8_t i = 0; i <= zoo_stat_count; i++, stat++) {
		if (stat->x == x && stat->y == y)
			return i;
	}

	return STAT_ID_NONE;
#endif
}

void add_stat(uint8_t tx, uint8_t ty, uint8_t element, uint8_t color, uint8_t cycle, const zoo_stat_t *template) {
	if (zoo_stat_count >= MAX_STAT) return;
	zoo_stat_count++;

	zoo_stat_t *dest_stat = &ZOO_STAT(zoo_stat_count);
	memcpy(dest_stat, template, sizeof(zoo_stat_t));
	dest_stat->x = tx;
	dest_stat->y = ty;
	dest_stat->cycle = cycle;
	if (ZOO_TILE_READBOUNDS(tx, ty)) {
		ZOO_TILE_COPY(dest_stat->under, ZOO_TILE(tx, ty));
	} else {
		dest_stat->under.element = 0;
		dest_stat->under.color = 0;
	}
	dest_stat->data_pos = 0;

	if (template->data_ofs != 0xFFFF) {
		dest_stat->data_ofs = oop_dataofs_clone(template->data_ofs);
	}

	if (!ZOO_TILE_WRITEBOUNDS(tx, ty)) return;

	if (zoo_element_defs_flags[dest_stat->under.element] & ELEMENT_PLACEABLE_ON_TOP) {
		ZOO_TILE(tx, ty).color = (color & 0x0F) | ((dest_stat->under.color) & 0x70);
	} else {
		ZOO_TILE(tx, ty).color = color;
	}
	ZOO_TILE(tx, ty).element = element;

	board_draw_tile(tx, ty);
}

extern uint8_t oop_stat_id;

void remove_stat(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

#ifdef DEBUG_PRINTFS
	EMU_printf("removing stat %u @ %u %u", stat_id, stat->x, stat->y);
#endif
	if (stat->data_ofs != 0xFFFF) {
		oop_dataofs_free_if_unused(stat->data_ofs, stat_id);
		stat->data_ofs = 0xFFFF;
	}

	if (stat_id < zoo_game_state.current_stat_ticked) zoo_game_state.current_stat_ticked--;
#ifdef BUGFIX_DIEMOVE_OOP_EXEC
	if (stat_id < oop_stat_id) oop_stat_id--;
#endif

	if (ZOO_TILE_WRITEBOUNDS(stat->x, stat->y)) {
		ZOO_TILE_COPY(ZOO_TILE(stat->x, stat->y), stat->under);
		board_draw_tile(stat->x, stat->y);
	}

	for (uint8_t i = 1; i <= zoo_stat_count; i++) {
		zoo_stat_t *cstat = &ZOO_STAT(i);
		if (cstat->follower > stat_id && cstat->follower < STAT_ID_NONE_MIN) {
			cstat->follower--;
		} else if (cstat->follower == stat_id) {
			cstat->follower = STAT_ID_NONE;
		}
		if (cstat->leader > stat_id && cstat->leader < STAT_ID_NONE_MIN) {
			cstat->leader--;
		} else if (cstat->leader == stat_id) {
			cstat->leader = STAT_ID_NONE;
		}
	}

	memmove(zoo_stats + stat_id + 1, zoo_stats + stat_id + 2, (zoo_stat_count - stat_id) * sizeof(zoo_stat_t));
	zoo_stat_count--;
}

bool move_stat_enable_scroll = true;

void move_stat(uint8_t stat_id, uint8_t new_x, uint8_t new_y) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t old_x = stat->x;
	uint8_t old_y = stat->y;

	zoo_tile_t old_under;
	ZOO_TILE_COPY(old_under, stat->under);
	if (ZOO_TILE_READBOUNDS(new_x, new_y)) {
		ZOO_TILE_ASSIGN(stat->under, new_x, new_y);
	} else {
#ifdef DEBUG_PRINTFS
		EMU_printf("stat %u oob move! %u %u -> %u %u", stat_id, old_x, old_y, new_x, new_y);
#endif
		stat->under.element = 0;
		stat->under.color = 0;
	}

	zoo_tile_t old_tile = {0, 0};
	zoo_tile_t new_tile;
	if (ZOO_TILE_READBOUNDS(old_x, old_y)) {
		ZOO_TILE_ASSIGN(old_tile, old_x, old_y);
	}
	ZOO_TILE_COPY(new_tile, stat->under);

	uint8_t new_color;
	if (old_tile.element == E_PLAYER) {
		new_color = old_tile.color;
	} else if (new_tile.element == E_EMPTY) {
		new_color = old_tile.color & 0x0F;
	} else {
		new_color = (old_tile.color & 0x0F) | (new_tile.color & 0x70);
	}

	if (ZOO_TILE_WRITEBOUNDS(new_x, new_y)) {
		ZOO_TILE_CHANGE2(new_x, new_y, old_tile.element, new_color);
	}

	stat->x = new_x;
	stat->y = new_y;

	board_draw_tile(new_x, new_y);

	if (ZOO_TILE_WRITEBOUNDS(old_x, old_y)) {
		ZOO_TILE_COPY(ZOO_TILE(old_x, old_y), old_under);
		board_draw_tile(old_x, old_y);
	}

	if (stat_id == 0 && move_stat_enable_scroll) {
		move_stat_scroll_stat0(old_x, old_y, new_x, new_y, false);
	}
}

void damage_stat_stat0(zoo_stat_t *stat, zoo_tile_t *tile);

void damage_stat(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	zoo_tile_t *tile = &ZOO_TILE(stat->x, stat->y);

	if (stat_id == 0) {
		uint8_t prev_bank = _current_bank;
		ZOO_SWITCH_ROM(2);
		damage_stat_stat0(stat, tile);
		ZOO_SWITCH_ROM(prev_bank);
	} else {
		switch (tile->element) {
			case E_BULLET:
				sound_queue(2, sound_destroy_bullet);
				break;
			case E_OBJECT:
				break;
			default:
				sound_queue(2, sound_destroy);
				break;
		}
		remove_stat(stat_id);
	}
}

void board_damage_tile(uint8_t x, uint8_t y) {
	uint8_t stat_id = get_stat_id_at(x, y);
	if (stat_id != STAT_ID_NONE) {
		damage_stat(stat_id);
	} else {
		if (ZOO_TILE_WRITEBOUNDS(x, y)) {
			ZOO_TILE(x, y).element = E_EMPTY;
			board_draw_tile(x, y);
		}
	}
}

void calc_direction_rnd(int8_t *dx, int8_t *dy) {
	*dx = rand(3) - 1;
	if (*dx == 0) {
		*dy = (RAND2() << 1) - 1;
	} else {
		*dy = 0;
	}
}

void calc_direction_seek(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	*dx = 0;
	*dy = 0;
	if ((RAND2() < 1) || (ZOO_STAT(0).y == y)) {
		*dx = signum8(ZOO_STAT(0).x - x);
	}
	if (*dx == 0) {
		*dy = signum8(ZOO_STAT(0).y - y);
	}
	if (zoo_world_info.energizer_ticks > 0) {
		*dx = -(*dx);
		*dy = -(*dy);
	}
}

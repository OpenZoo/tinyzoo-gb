#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "gamevars.h"
#include "game.h"
#include "elements.h"
#include "input.h"
#include "math.h"
#include "renderer.h"
#include "sound_consts.h"
#include "timer.h"

static const zoo_tile_t TileNormal = {E_NORMAL, 0x0E};
static const zoo_stat_t StatCreateMinusOne = {
	0, 1,
	0, 0, 0,
	0, 1, 0,
	1, 1,
	{1, 0}
};
static const zoo_stat_t StatCreatePlayer = {
	BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1,
	0, 0, 1,
	0, 0, 0,
	STAT_ID_NONE, STAT_ID_NONE,
	{0, 0}
};

const zoo_stat_t stat_template_default = {
	0, 0,
	0, 0, 0,
	0, 0, 0,
	STAT_ID_NONE, STAT_ID_NONE,
	{0, 0}
};

const uint8_t line_tiles[16] = {
	249, 208, 210, 186,
	181, 188, 187, 185,
	198, 200, 201, 204,
	205, 202, 203, 206
};

const int8_t neighbor_delta_x[4] = {0, 0, -1, 1};
const int8_t neighbor_delta_y[4] = {-1, 1, 0, 0};
const int8_t diagonal_delta_x[8] = {-1, 0, 1, 1, 1, 0, -1, -1};
const int8_t diagonal_delta_y[8] = {1, 1, 1, 0, -1, -1, -1, 0};

static uint8_t viewport_x = 1;
static uint8_t viewport_y = 1;

#define VIEWPORT_MIN_X 1
#define VIEWPORT_MAX_X (BOARD_WIDTH - VIEWPORT_WIDTH + 1)
#define VIEWPORT_MIN_Y 1
#define VIEWPORT_MAX_Y (BOARD_HEIGHT - VIEWPORT_HEIGHT + 1)

void center_viewport_on_player(void) {
	int8_t vx = ZOO_STAT(0).x - VIEWPORT_CENTER_X;
	int8_t vy = ZOO_STAT(0).y - VIEWPORT_CENTER_Y;
	if (vx < VIEWPORT_MIN_X) vx = VIEWPORT_MIN_X;
	if (vx >= VIEWPORT_MAX_X) vx = VIEWPORT_MAX_X;
	if (vy < VIEWPORT_MIN_Y) vy = VIEWPORT_MIN_Y;
	if (vy >= VIEWPORT_MAX_Y) vy = VIEWPORT_MAX_Y;

	viewport_x = vx;
	viewport_y = vy;
}

void board_create(void) {
	memset(&zoo_board_info, 0, sizeof(zoo_board_info));
	memset(zoo_tiles, 0, sizeof(zoo_tile_t) * 64 * 27);
	
	zoo_board_info.max_shots = 255;

	for (uint8_t iy = 0; iy <= BOARD_HEIGHT + 1; iy++) {
		ZOO_TILE(0, iy).element = E_BOARD_EDGE;
		ZOO_TILE(BOARD_WIDTH + 1, iy).element = E_BOARD_EDGE;
	}
	for (uint8_t ix = 0; ix <= BOARD_WIDTH; ix++) {
		ZOO_TILE(ix, 0).element = E_BOARD_EDGE;
		ZOO_TILE(ix, BOARD_HEIGHT + 1).element = E_BOARD_EDGE;
	}
	for (uint8_t iy = 1; iy <= BOARD_HEIGHT; iy++) {
		ZOO_TILE_COPY(ZOO_TILE(1, iy), TileNormal);
		ZOO_TILE_COPY(ZOO_TILE(BOARD_WIDTH, iy), TileNormal);
	}
	for (uint8_t ix = 2; ix < BOARD_WIDTH; ix++) {
		ZOO_TILE_COPY(ZOO_TILE(ix, 1), TileNormal);
		ZOO_TILE_COPY(ZOO_TILE(ix, BOARD_HEIGHT), TileNormal);
	}

	zoo_stat_count = 0;
	ZOO_STAT(-1) = StatCreateMinusOne; 

	ZOO_TILE(BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1).element = E_PLAYER;
	ZOO_TILE(BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1).color = 0x1F; // TODO

	ZOO_STAT(0) = StatCreatePlayer;
	center_viewport_on_player();
}

void world_create(void) {
	// TODO: boardcount, boardlen, messagenotshownflags
	board_create();
	memset(&zoo_world_info, 0, sizeof(zoo_world_info));
	zoo_world_info.health = 100;
	// TODO: boardchange
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
		const zoo_element_def_t *def = zoo_element_defs + tile.element;
		if (!(def->flags & ELEMENT_VISIBLE_IN_DARK)) {
			if (zoo_world_info.torch_ticks > 0) {
				int16_t dx = ZOO_STAT(0).x - x;
				int16_t dy = ZOO_STAT(0).y - y;
				int16_t dist = (dx*dx) + (dy*dy*2);
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
	} else if (tile.element < E_TEXT_MIN) {
		const zoo_element_def_t *def = zoo_element_defs + tile.element;
		uint8_t ch;
		if (def->draw_proc != 0) {
			ch = def->draw_proc(x, y);
		} else {
			ch = def->character;
		}

		text_draw(vx, vy, ch, tile.color);
	} else if (tile.element == E_TEXT_WHITE) {
		text_draw(vx, vy, tile.color, 0x0F);
	} else {
		text_draw(vx, vy, tile.color, 0x0F | ((tile.element - E_TEXT_MIN + 1) << 4));
	}
}

void board_redraw(void) {
	for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
		for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
			board_draw_tile(ix + viewport_x, iy + viewport_y);
		}
	}
}

void game_play_loop(bool board_changed) {
	// TODO

	// TODO ZOO_TILE(ZOO_STAT(0).x, ZOO_STAT(0).y).element

	if (board_changed) {
		board_redraw();
	}

	zoo_game_state.play_exit_requested = false;

	zoo_game_state.current_tick = rand(100);
	zoo_game_state.current_stat_ticked = zoo_stat_count + 1;
	do {
		// TODO: if paused
		if (zoo_game_state.current_stat_ticked <= zoo_stat_count) {
			zoo_stat_t *stat = &ZOO_STAT(zoo_game_state.current_stat_ticked);
			if (stat->x <= (BOARD_WIDTH + 1) && stat->y <= (BOARD_HEIGHT + 1)) {
				const zoo_element_def_t *element = &zoo_element_defs[ZOO_TILE(stat->x, stat->y).element];

				if (element->tick_proc != 0 && stat->cycle != 0) {
					// if (stat->cycle == 1 || ((zoo_game_state.current_tick % stat->cycle) == (zoo_game_state.current_stat_ticked % stat->cycle))) {
					if (stat->cycle == 1 || (((int16_t) zoo_game_state.current_tick - zoo_game_state.current_stat_ticked) % stat->cycle) == 0) {
						element->tick_proc(zoo_game_state.current_stat_ticked);
					}
				}
			}

			zoo_game_state.current_stat_ticked++;
		}

		if (zoo_game_state.current_stat_ticked > zoo_stat_count && !zoo_game_state.play_exit_requested) {
			text_update();
			
			while (!timer_has_time_elapsed(&zoo_game_state.tick_time_counter, 8)) {
				// TODO: stop
			}

			if (zoo_game_state.current_tick >= 420) zoo_game_state.current_tick = 1;
			else zoo_game_state.current_tick++;

			zoo_game_state.current_stat_ticked = 0;
			
			input_update();
		}
	} while(!zoo_game_state.play_exit_requested);
}

uint8_t get_stat_id_at(uint8_t x, uint8_t y) {
	zoo_stat_t *stat = zoo_stats + 1;

	for (uint8_t i = 0; i <= zoo_stat_count; i++, stat++) {
		if (stat->x == x && stat->y == y)
			return i;
	}

	return STAT_ID_NONE;
}

void add_stat(uint8_t tx, uint8_t ty, uint8_t element, uint8_t color, uint8_t cycle, const zoo_stat_t *template) {
	if (zoo_stat_count >= MAX_STAT) return;

	zoo_stat_count++;
	zoo_stat_t *dest_stat = &ZOO_STAT(zoo_stat_count);
	memcpy(dest_stat, template, sizeof(zoo_stat_t));
	dest_stat->x = tx;
	dest_stat->y = ty;
	dest_stat->cycle = cycle;
	ZOO_TILE_COPY(dest_stat->under, ZOO_TILE(tx, ty));
	// TODO: datapos

	// TODO: data

	if (zoo_element_defs[dest_stat->under.element].flags & ELEMENT_PLACEABLE_ON_TOP) {
		ZOO_TILE(tx, ty).color = (color & 0x0F) | ((dest_stat->under.color) & 0x70);
	} else {
		ZOO_TILE(tx, ty).color = color;
	}
	ZOO_TILE(tx, ty).element = element;

	board_draw_tile(tx, ty);
}

void remove_stat(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	// TODO: stat data handling
	if (stat_id < zoo_game_state.current_stat_ticked) zoo_game_state.current_stat_ticked--;

	ZOO_TILE_COPY(ZOO_TILE(stat->x, stat->y), stat->under);
	board_draw_tile(stat->x, stat->y);

	// TODO: centipede freeing

	memmove(zoo_stats + stat_id + 1, zoo_stats + stat_id + 2, (zoo_stat_count - stat_id) * sizeof(zoo_stat_t));
	zoo_stat_count--;
}

void move_stat(uint8_t stat_id, uint8_t new_x, uint8_t new_y) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t old_x = stat->x;
	uint8_t old_y = stat->y;

	zoo_tile_t old_under;
	ZOO_TILE_COPY(old_under, stat->under);
	ZOO_TILE_ASSIGN(stat->under, new_x, new_y);

	zoo_tile_t old_tile;
	zoo_tile_t new_tile;
	ZOO_TILE_ASSIGN(old_tile, old_x, old_y);
	ZOO_TILE_COPY(new_tile, stat->under);

	uint8_t new_color;
	if (old_tile.element == E_PLAYER) {
		new_color = old_tile.color;
	} else if (new_tile.element == E_EMPTY) {
		new_color = old_tile.color & 0x0F;
	} else {
		new_color = (old_tile.color & 0x0F) | (new_tile.color & 0x70);
	}

	ZOO_TILE(new_x, new_y).element = old_tile.element;
	ZOO_TILE(new_x, new_y).color = new_color;
	ZOO_TILE_COPY(ZOO_TILE(old_x, old_y), old_under);

	stat->x = new_x;
	stat->y = new_y;

	board_draw_tile(new_x, new_y);
	board_draw_tile(old_x, old_y);

	if (stat_id == 0) {
		if ((zoo_board_info.flags & BOARD_IS_DARK) && (zoo_world_info.torch_ticks > 0)) {
			// TODO: optimize
			board_redraw();
		} else {
			// move viewport?
			int8_t pox = new_x - viewport_x;
			int8_t poy = new_y - viewport_y;
			int8_t dist = difference8(old_x, new_x) + difference8(old_y, new_y);

			if (dist > 1) {
				// full viewport adjust
				// TODO: optimize?
				uint8_t ov_x = viewport_x;
				uint8_t ov_y = viewport_y;
				center_viewport_on_player();
				int8_t vd_y = viewport_y - ov_y;
				if (viewport_x == ov_x) {
					text_scroll(0, vd_y);
					if (vd_y < 0) {
						for (uint8_t iy = 0; iy < -vd_y; iy++) {
							for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
								board_draw_tile(ix + viewport_x, iy + viewport_y);
							}
						}
					} else {
						for (uint8_t iy = 0; iy < vd_y; iy++) {
							for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
								board_draw_tile(ix + viewport_x, viewport_y + VIEWPORT_HEIGHT - 1 - iy);
							}
						}
					}
				} else {
					text_mark_redraw();
					board_redraw();
				}
			} else if (pox == VIEWPORT_PLAYER_MIN_X-1) {
				// move left
				if (viewport_x > VIEWPORT_MIN_X) {
					viewport_x--;
					text_scroll(-1, 0);
					for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
						board_draw_tile(viewport_x, iy + viewport_y);
					}
				}
			} else if (pox == VIEWPORT_PLAYER_MAX_X+1) {
				// move left
				if (viewport_x < VIEWPORT_MAX_X) {
					viewport_x++;
					text_scroll(1, 0);
					for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
						board_draw_tile(viewport_x + VIEWPORT_WIDTH - 1, iy + viewport_y);
					}
				}
			} else if (poy == VIEWPORT_PLAYER_MIN_Y-1) {
				// move up
				if (viewport_y > VIEWPORT_MIN_Y) {
					viewport_y--;
					text_scroll(0, -1);
					for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
						board_draw_tile(ix + viewport_x, viewport_y);
					}
				}
			} else if (poy == VIEWPORT_PLAYER_MAX_Y+1) {
				// move down
				if (viewport_y < VIEWPORT_MAX_Y) {
					viewport_y++;
					text_scroll(0, 1);
					for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
						board_draw_tile(ix + viewport_x, viewport_y + VIEWPORT_HEIGHT - 1);
					}
				}
			}
		}
	}
}

bool board_shoot(uint8_t element, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t source) {
	zoo_tile_t ntile;
	ZOO_TILE_COPY(ntile, ZOO_TILE(x + dx, y + dy));

	if ((zoo_element_defs[ntile.element].flags & ELEMENT_WALKABLE) || (ntile.element == E_WATER)) {
		add_stat(x + dx, y + dy, element, zoo_element_defs[element].color, 1, &stat_template_default);

		zoo_stat_t *new_stat = &ZOO_STAT(zoo_stat_count);
		new_stat->p1 = source;
		new_stat->step_x = dx;
		new_stat->step_y = dy;
		new_stat->p2 = 100;

		return true;
	} else if ((ntile.element == E_BREAKABLE) || (
		(zoo_element_defs[ntile.element].flags & ELEMENT_DESTRUCTIBLE)
		&& ((ntile.element == E_PLAYER) == source)
		&& (zoo_world_info.energizer_ticks <= 0)
	)) {
		board_damage_tile(x + dx, y + dy);
		sound_queue(2, sound_damage);
		return true;
	} else {
		return false;
	}
}

void damage_stat(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	zoo_tile_t *tile = &ZOO_TILE(stat->x, stat->y);

	if (stat_id == 0) {
		if (zoo_world_info.health > 0) {
			zoo_world_info.health -= 10;

			game_update_sidebar();
			// TODO: DisplayMessage

			tile->color = 0x70 | (zoo_element_defs[E_PLAYER].color & 0xF);

			if (zoo_world_info.health > 0) {
				zoo_world_info.board_time_sec = 0;
				if (zoo_board_info.flags & BOARD_REENTER_WHEN_ZAPPED) {
					sound_queue(4, sound_player_zapped);

					uint8_t old_x = stat->x;
					uint8_t old_y = stat->y;
					tile->element = E_EMPTY;
					board_draw_tile(old_x, old_y);
					stat->x = zoo_board_info.start_player_x;
					stat->y = zoo_board_info.start_player_y;
					board_redraw();

					zoo_game_state.paused = true;
				}
				sound_queue(4, sound_player_damage);
			} else {
				sound_queue(5, sound_player_game_over);
			}
		}
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
		ZOO_TILE(x, y).element = E_EMPTY;
		board_draw_tile(x, y);
	}
}

void game_update_sidebar(void) {
	// TODO
}

void board_attack(uint8_t stat_id, uint8_t x, uint8_t y) {
	if (stat_id == 0 && zoo_world_info.energizer_ticks > 0) {
		zoo_world_info.score += zoo_element_defs[ZOO_TILE(x, y).element].score_value;
		game_update_sidebar();
	} else {
		damage_stat(stat_id);
	}

	if (stat_id > 0 && stat_id <= zoo_game_state.current_stat_ticked) {
		zoo_game_state.current_stat_ticked--;
	}

	if ((ZOO_TILE(x, y).element == E_PLAYER) && zoo_world_info.energizer_ticks > 0) {
		zoo_stat_t *attacker_stat = &ZOO_STAT(stat_id);
		zoo_world_info.score += zoo_element_defs[ZOO_TILE(attacker_stat->x, attacker_stat->y).element].score_value;
		game_update_sidebar();
	} else {
		board_damage_tile(x, y);
		sound_queue(2, sound_damage);
	}
}

void calc_direction_rnd(int8_t *dx, int8_t *dy) {
	*dx = rand(3) - 1;
	if (*dx == 0) {
		*dy = (rand(2) << 1) - 1;
	} else {
		*dy = 0;
	}
}

void calc_direction_seek(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	*dx = 0;
	*dy = 0;
	if ((rand(2) < 1) || (ZOO_STAT(0).y == y)) {
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

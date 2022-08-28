#pragma bank 2

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gbdk/platform.h>
#include "board_manager.h"
#include "gamevars.h"
#include "game.h"
#include "elements.h"
#include "input.h"
#include "math.h"
#include "../res/message_consts.h"
#include "oop.h"
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

extern int8_t viewport_x;
extern int8_t viewport_y;

void board_create(void) BANKED {
	memset(&zoo_board_info, 0, sizeof(zoo_board_info));
	memset(zoo_tiles, 0, sizeof(zoo_tiles));
	
	zoo_board_info.max_shots = 255;

#ifndef OPT_BOARD_EDGE_IMMUTABLE
	for (uint8_t iy = 0; iy <= BOARD_HEIGHT + 1; iy++) {
#else
	for (uint8_t iy = 0; iy <= BOARD_HEIGHT; iy++) {
#endif
		ZOO_TILE(0, iy).element = E_BOARD_EDGE;
		ZOO_TILE(BOARD_WIDTH + 1, iy).element = E_BOARD_EDGE;
	}
	for (uint8_t ix = 1; ix <= BOARD_WIDTH; ix++) {
		ZOO_TILE(ix, 0).element = E_BOARD_EDGE;
#ifndef OPT_BOARD_EDGE_IMMUTABLE
		ZOO_TILE(ix, BOARD_HEIGHT + 1).element = E_BOARD_EDGE;
#endif
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
	viewport_reset();
}

void world_create(void) BANKED {
	// TODO: boardcount, boardlen
	memset(&msg_flags, 0, sizeof(msg_flags));
	board_create();
	memset(&zoo_world_info, 0, sizeof(zoo_world_info));
	zoo_world_info.health = 100;
	memset(&zoo_world_info.oop_flags, FLAG_ID_NONE, sizeof(zoo_world_info.oop_flags));
	// TODO: This will save to SRAM - we don't want this!
	// board_change(0);
}

void damage_stat_stat0(zoo_tile_t *tile) {
	if (zoo_world_info.health > 0) {
#ifndef HACK_PLAYER_INVINCIBLE
		zoo_world_info.health -= 10;

		game_update_sidebar_health();
#endif

		tile->color = 0x70 | (zoo_element_defs_color[E_PLAYER] & 0xF);

		if (zoo_world_info.health > 0) {
			zoo_world_info.board_time_sec = 0;
			if (zoo_board_info.flags & BOARD_REENTER_WHEN_ZAPPED) {
				sound_queue(4, sound_player_zapped);

				uint8_t old_x = ZOO_STAT(0).x;
				uint8_t old_y = ZOO_STAT(0).y;
				tile->element = E_EMPTY;
				board_draw_tile(old_x, old_y);
				ZOO_STAT(0).x = zoo_board_info.start_player_x;
				ZOO_STAT(0).y = zoo_board_info.start_player_y;
				// center_viewport_on_player();
				// board_redraw();
				if (viewport_request_player_focus()) {
					move_stat_scroll_focused(0, old_x, old_y, ZOO_STAT(0).x, ZOO_STAT(0).y, true);
				}
				text_update();

				zoo_game_state.paused = true;
			} else {
				// This used to apply all the time, but for the GB port
				// we're moving it here - tentatively, anyway?
				display_message(100, NULL, NULL, msg_ouch);
			}
			sound_queue(4, sound_player_damage);
		} else {
			sound_queue(5, sound_player_game_over);
		}
	}
}

void scroll_viewport_to(uint8_t vx, uint8_t vy, bool force_redraw) {
	int8_t pox = vx - viewport_x;
	int8_t poy = vy - viewport_y;
	uint8_t dist = abs(pox) + abs(poy);

	if (force_redraw || dist > MAX_SCROLL_DISTANCE_BEFORE_REDRAW) {
		viewport_x = vx;
		viewport_y = vy;
		board_redraw();
	} else {
		renderer_scrolling = 1;
		if (pox < 0) {
			// move left
			pox = -pox;
			for (uint8_t ix = 0; ix < (uint8_t)pox; ix++) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_undraw_tile(viewport_x + VIEWPORT_WIDTH - 1 - ix, iy + viewport_y);
				}
			}
			viewport_x -= pox;
			text_scroll(-pox, 0);
			for (uint8_t ix = 0; ix < (uint8_t)pox; ix++) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_draw_tile(viewport_x + ix, iy + viewport_y);
				}
			}
		} else if (pox > 0) {
			// move right
			for (uint8_t ix = 0; ix < (uint8_t)pox; ix++) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_undraw_tile(viewport_x + ix, iy + viewport_y);
				}
			}
			viewport_x += pox;
			text_scroll(pox, 0);
			for (uint8_t ix = 0; ix < (uint8_t)pox; ix++) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_draw_tile(viewport_x + VIEWPORT_WIDTH - 1 - ix, iy + viewport_y);
				}
			}
		}
		if (poy < 0) {
			// move up
			viewport_y += poy;
			text_scroll(0, poy);
			poy = -poy;
			for (uint8_t iy = 0; iy < (uint8_t)poy; iy++) {
				text_free_line(iy);
				for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
					board_draw_tile(ix + viewport_x, iy + viewport_y);
				}
			}
		} else if (poy > 0) {
			// move down
			viewport_y += poy;
			text_scroll(0, poy);
			for (uint8_t iy = 0; iy < (uint8_t)poy; iy++) {
				text_free_line(VIEWPORT_HEIGHT - 1 - iy);
				for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
					board_draw_tile(ix + viewport_x, viewport_y + VIEWPORT_HEIGHT - 1 - iy);
				}
			}
		}
		renderer_scrolling = 0;
		bool is_dark = (zoo_board_info.flags & BOARD_IS_DARK) && (zoo_world_info.torch_ticks > 0);
		if (is_dark) {
			// redraw tiles around the player, plus one
			uint8_t py = ZOO_STAT(0).y - viewport_y;
			for (int8_t iy = -TORCH_DY-1; iy <= TORCH_DY+1; iy++) {
				// text_free_line(py + iy);
				for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
					board_draw_tile(ix + viewport_x, ZOO_STAT(0).y + iy);
				}
			}
		}
	}
}

void game_scrolling_view(void) BANKED {
	int8_t old_vx = viewport_x;
	int8_t old_vy = viewport_y;

	input_wait_clear();
	wait_vbl_done();
	text_reinit(RENDER_MODE_TITLE);

	while (true) {
		wait_vbl_done();
		wait_vbl_done();

		input_update();
		if (input_keys & 0xF0) break;

		int8_t vx = viewport_x + input_delta_x;
		int8_t vy = viewport_y + input_delta_y;
		if (vx < VIEWPORT_MIN_X) vx = VIEWPORT_MIN_X;
		else if (vx > VIEWPORT_MAX_X) vx = VIEWPORT_MAX_X;
		if (vy < VIEWPORT_MIN_Y) vy = VIEWPORT_MIN_Y;
		else if (vy > VIEWPORT_MAX_Y) vy = VIEWPORT_MAX_Y;

		if (vx != viewport_x || vy != viewport_y) {
			scroll_viewport_to(vx, vy, false);
			text_update();
		}
	}

	input_wait_clear();

	if (old_vx != viewport_x || old_vy != viewport_y) {
		scroll_viewport_to(old_vx, old_vy, false);
	}

	wait_vbl_done();
	text_reinit(RENDER_MODE_PLAYFIELD);
}

void move_stat_scroll_focused(uint8_t stat_id, uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y, bool force) BANKED {
	bool force_redraw = false;
	// move viewport?
	int8_t ovx = viewport_x;
	int8_t ovy = viewport_y;
	int8_t vx = viewport_x;
	int8_t vy = viewport_y;
	int8_t pox = new_x - vx;
	int8_t poy = new_y - vy;
	bool recalc_required = false;

	if (viewport_focus_stat != 0 || force) {
		recalc_required = true;
		if (!viewport_focus_locked) {
			viewport_focus_stat = stat_id;
		}
	} else if (pox < VIEWPORT_PLAYER_MIN_X && vx > VIEWPORT_MIN_X) {
		if ((old_x - 1) == new_x) {
			vx--;
			if (vx < VIEWPORT_MIN_X) vx = VIEWPORT_MIN_X;
		} else {
			recalc_required = true;
		}
	} else if (pox > VIEWPORT_PLAYER_MAX_X && vx < VIEWPORT_MAX_X) {
		if ((old_x + 1) == new_x) {
			vx++;
			if (vx > VIEWPORT_MAX_X) vx = VIEWPORT_MAX_X;
		} else {
			recalc_required = true;
		}
	}

	if (poy < VIEWPORT_PLAYER_MIN_Y && vy > VIEWPORT_MIN_Y) {
		if ((old_y - 1) == new_y) {
			vy--;
			if (vy < VIEWPORT_MIN_Y) vy = VIEWPORT_MIN_Y;
		} else {
			recalc_required = true;
		}
	} else if (poy > VIEWPORT_PLAYER_MAX_Y && vy < VIEWPORT_MAX_Y) {
		if ((old_y + 1) == new_y) {
			vy++;
			if (vy > VIEWPORT_MAX_Y) vy = VIEWPORT_MAX_Y;
		} else {
			recalc_required = true;
		}
	}

	if (recalc_required) {
		center_viewport_on(new_x, new_y);
		vx = viewport_x;
		viewport_x = ovx;
		vy = viewport_y;
		viewport_y = ovy;
	}

	bool is_dark = (zoo_board_info.flags & BOARD_IS_DARK) && (zoo_world_info.torch_ticks > 0);
	if (force_redraw || is_dark || ovx != vx || ovy != vy) {
		scroll_viewport_to(vx, vy, force_redraw);
	}

	renderer_scrolling = 0;
}

bool board_shoot(uint8_t element, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t source) BANKED {
	zoo_tile_t ntile;
	if (!ZOO_TILE_WRITEBOUNDS(x + dx, y + dy)) return false;

	ZOO_TILE_COPY(ntile, ZOO_TILE(x + dx, y + dy));

	if ((zoo_element_defs_flags[ntile.element] & ELEMENT_WALKABLE) || (ntile.element == E_WATER)) {
		add_stat(x + dx, y + dy, element, zoo_element_defs_color[element], 1, &stat_template_default);

		zoo_stat_t *new_stat = &ZOO_STAT(zoo_stat_count);
		new_stat->p1 = source;
		new_stat->step_x = dx;
		new_stat->step_y = dy;
		new_stat->p2 = 100;

		return true;
	} else if ((ntile.element == E_BREAKABLE) || (
		(zoo_element_defs_flags[ntile.element] & ELEMENT_DESTRUCTIBLE)
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

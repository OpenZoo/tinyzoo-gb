#pragma bank 1

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

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../game_transition.h"
#include "../input.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

extern bool move_stat_enable_scroll;
extern int8_t viewport_x;
extern int8_t viewport_y;

void ElementBoardEdgeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	uint8_t entry_x = ZOO_STAT(0).x;
	uint8_t entry_y = ZOO_STAT(0).y;
	uint8_t neighbor_id;

	if (*dy == -1) {
		neighbor_id = 0;
		entry_y = BOARD_HEIGHT;
	} else if (*dy == 1) {
		neighbor_id = 1;
		entry_y = 1;
	} else if (*dx == -1) {
		neighbor_id = 2;
		entry_x = BOARD_WIDTH;
	} else {
		neighbor_id = 3;
		entry_x = 1;
	}

	if (zoo_board_info.neighbor_boards[neighbor_id] != 0) {
		// hide player during change
		zoo_tile_t tile_player;
		ZOO_TILE_ASSIGN(tile_player, ZOO_STAT(0).x, ZOO_STAT(0).y);
		ZOO_TILE_CHANGE2(ZOO_STAT(0).x, ZOO_STAT(0).y, ZOO_STAT(0).under.element, ZOO_STAT(0).under.color);
		board_draw_tile(ZOO_STAT(0).x, ZOO_STAT(0).y);
		ZOO_TILE_CHANGE2(ZOO_STAT(0).x, ZOO_STAT(0).y, tile_player.element, tile_player.color);

		uint8_t prev_board_id = zoo_world_info.current_board;
		board_change(zoo_board_info.neighbor_boards[neighbor_id]);

		zoo_tile_t *entry_tile = &ZOO_TILE(entry_x, entry_y);
		if (entry_tile->element != E_PLAYER) {
			zoo_element_defs_touchprocs[entry_tile->element]
				(entry_x, entry_y, &input_delta_x, &input_delta_y);
		}

		if ((zoo_element_defs_flags[entry_tile->element] & ELEMENT_WALKABLE) || (entry_tile->element == E_PLAYER)) {
			if (entry_tile->element != E_PLAYER) {
				move_stat_enable_scroll = false;
				move_stat(0, entry_x, entry_y);
				move_stat_enable_scroll = true;
			}

			*dx = 0;
			*dy = 0;
			board_enter_stage1();
			board_enter_stage2();
			board_redraw();
			board_enter_stage3();
			return;
		} else {
			board_change(prev_board_id);
		}

		// redraw player (was hidden)
		board_draw_tile(ZOO_STAT(0).x, ZOO_STAT(0).y);
	}
}

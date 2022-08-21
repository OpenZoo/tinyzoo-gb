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
#include "../input.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t duplicator_tiles[6] = {250, 250, 249, 248, 111, 79};

uint8_t ElementDuplicatorDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 >= 6) p1 = 0;
	return duplicator_tiles[p1];
}

// Warning: No READBOUNDS
void ElementDuplicatorTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t sx = stat->x;
	uint8_t sy = stat->y;
#ifdef BUGFIX_KOOPO_BUG
	uint8_t last_current_board = zoo_world_info.current_board;
#endif

	if (stat->p1 <= 4) {
		stat->p1++;
	} else {
		stat->p1 = 0;
		int8_t stepx = stat->step_x;
		int8_t stepy = stat->step_y;
		uint8_t src_elem = ZOO_TILE(sx - stepx, sy - stepy).element;
		if (src_elem == E_PLAYER) {
			uint8_t dst_elem = ZOO_TILE(sx + stepx, sy + stepy).element;
			zoo_element_defs_touchprocs[dst_elem](sx + stepx, sy + stepy, &input_delta_x, &input_delta_y);
		} else {
			if (src_elem != E_EMPTY) {
				ElementPushablePush(sx - stepx, sy - stepy, -stepx, -stepy);
 				src_elem = ZOO_TILE(sx - stepx, sy - stepy).element;
 			}

			if (src_elem == E_EMPTY) {
				uint8_t source_stat_id = get_stat_id_at(sx + stepx, sy + stepy);
				if (source_stat_id == 255) {
					ZOO_TILE_COPY(
						ZOO_TILE(sx - stepx, sy - stepy),
						ZOO_TILE(sx + stepx, sy + stepy)
					);
				} else if (source_stat_id != 0) {
					zoo_tile_t src_tile;
					ZOO_TILE_ASSIGN(src_tile, sx + stepx, sy + stepy);
					zoo_stat_t *src_stat = &ZOO_STAT(source_stat_id);

					add_stat(sx - stepx, sy - stepy,
						src_tile.element, src_tile.color,
						src_stat->cycle, src_stat);
				} else {
					goto PostPlace;
				}
				board_draw_tile(sx - stepx, sy - stepy);

PostPlace:
				sound_queue(3, sound_duplicator_success);
			} else {
				sound_queue(3, sound_duplicator_failure);
			}
		}
#ifdef BUGFIX_KOOPO_BUG
		if (last_current_board != zoo_world_info.current_board) return;
#endif
		stat->p1 = 0;
	}
	
	board_draw_tile(sx, sy);
	stat->cycle = (9 - stat->p2) * 3;
}

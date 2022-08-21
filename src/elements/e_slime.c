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
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementSlimeTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	if (stat->p1 < stat->p2) {
		stat->p1++;
	} else {
		stat->p1 = 0;
		uint8_t start_x = stat->x;
		uint8_t start_y = stat->y;
		uint8_t color_copy = ZOO_TILE(start_x, start_y).color;
		bool changed_tiles = false;

		for (uint8_t dir = 0; dir < 4; dir++) {
			uint8_t tx = start_x + neighbor_delta_x[dir];
			uint8_t ty = start_y + neighbor_delta_y[dir];

			if (zoo_element_defs_flags[ZOO_TILE(tx, ty).element] & ELEMENT_WALKABLE) {
				if (!changed_tiles) {
					changed_tiles = true;
					move_stat(stat_id, tx, ty);
					ZOO_TILE_CHANGE2(start_x, start_y, E_BREAKABLE, color_copy);
					board_draw_tile(start_x, start_y);
				} else {
					add_stat(tx, ty, E_SLIME, color_copy, 3, &stat_template_default);
					ZOO_STAT(zoo_stat_count).p2 = stat->p2;
				}
			}
		}

		if (!changed_tiles) {
			remove_stat(stat_id);
			ZOO_TILE_CHANGE2(start_x, start_y, E_BREAKABLE, color_copy);
			board_draw_tile(start_x, start_y);
		}
	}
}

void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t color_copy = tile->color;
	damage_stat(get_stat_id_at(x, y));
	tile->element = E_BREAKABLE;
	tile->color = color_copy;

	board_draw_tile(x, y);
	sound_queue(2, sound_slime_touch);
}

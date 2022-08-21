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

static const uint8_t star_tiles[4] = {179, '/', 196, '\\'};

uint8_t ElementStarDraw(uint8_t x, uint8_t y) {
	uint8_t c = ZOO_TILE(x, y).color;
	if (c >= 15) { c = 9; } else { c++; }
	ZOO_TILE(x, y).color = c;
	return star_tiles[zoo_game_state.current_tick & 3];
}

void ElementStarTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t p2 = --stat->p2;
	if (p2 <= 0) {
		remove_stat(stat_id);
	} else {
		uint8_t sx = stat->x;
		uint8_t sy = stat->y;
		if (!(p2 & 1)) {
			calc_direction_seek(sx, sy, &stat->step_x, &stat->step_y);
			uint8_t* t_elem_ptr = &(ZOO_TILE(sx + stat->step_x, sy + stat->step_y).element);
			uint8_t t_elem = *t_elem_ptr;
			if (t_elem == E_PLAYER || t_elem == E_BREAKABLE) {
				board_attack(stat_id, sx + stat->step_x, sy + stat->step_y);
			} else {
				if (!(zoo_element_defs_flags[t_elem] & ELEMENT_WALKABLE)) {
					ElementPushablePush(sx + stat->step_x, sy + stat->step_y, stat->step_x, stat->step_y);
					t_elem = *t_elem_ptr;
				}

				if ((zoo_element_defs_flags[t_elem] & ELEMENT_WALKABLE) || (t_elem == E_WATER)) {
					move_stat(stat_id, sx + stat->step_x, sy + stat->step_y);
				}
			}
		} else {
			board_draw_tile(sx, sy);
		}
	}
}

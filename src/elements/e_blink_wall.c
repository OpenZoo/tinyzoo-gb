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

void ElementBlinkWallTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->p3 == 0) {
		stat->p3 = stat->p1 + 1;
	}

	if (stat->p3 == 1) {
		int8_t stepx = stat->step_x;
		int8_t stepy = stat->step_y;

		uint8_t ix = stat->x + stepx;
		uint8_t iy = stat->y + stepy;

		uint8_t elem = (stepx != 0) ? E_BLINK_RAY_EW : E_BLINK_RAY_NS;
		uint8_t color = ZOO_TILE(stat->x, stat->y).color;

		// clear ray
		bool cleared_ray = false;
		while (true) {
			if (!ZOO_TILE_READBOUNDS(ix, iy)) break;
			zoo_tile_t *tile = &ZOO_TILE(ix, iy);
			if (tile->element != elem || tile->color != color) {
				break;
			}
			tile->element = E_EMPTY;
			board_draw_tile(ix, iy);
			ix += stepx;
			iy += stepy;
			cleared_ray = true;
		}

		if (!cleared_ray) {
			// spawn new ray
			while (true) {
				if (!ZOO_TILE_READBOUNDS(ix, iy)) goto HitBoundary;

				zoo_tile_t *tile = &ZOO_TILE(ix, iy);
				if ((tile->element != E_EMPTY) && ((zoo_element_defs_flags[tile->element] & ELEMENT_DESTRUCTIBLE))) {
					board_damage_tile(ix, iy);
				}

				if (tile->element == E_PLAYER) {
					uint8_t player_stat_id = get_stat_id_at(ix, iy);

					if (stepx != 0) {
						if (ZOO_TILE(ix, iy - 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy - 1);
						} else if (ZOO_TILE(ix, iy + 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy + 1);
						}
					} else {
						if (ZOO_TILE(ix + 1, iy).element == E_EMPTY) {
							move_stat(player_stat_id, ix + 1, iy);
						} else if (ZOO_TILE(ix - 1, iy).element == E_EMPTY) {
#ifdef BUGFIX_BLINKWALL_OFFSET
							move_stat(player_stat_id, ix - 1, iy);
#else
							move_stat(player_stat_id, ix + 1, iy);
#endif
						}
					}

					if (tile->element == E_PLAYER) {
						while (zoo_world_info.health > 0)
							damage_stat(player_stat_id);
						goto HitBoundary;
					}
				}

				if (tile->element == E_EMPTY) {
					tile->element = elem;
					tile->color = color;
					board_draw_tile(ix, iy);
				} else {
					goto HitBoundary;
				}

				ix += stepx;
				iy += stepy;
			}
		}

HitBoundary:
		stat->p3 = stat->p2 * 2 + 1;
	} else {
		stat->p3--;
	}
}

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

static const uint8_t spinning_gun_tiles[4] = {24, 26, 25, 27};

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y) {
	return spinning_gun_tiles[(zoo_game_state.current_tick >> 1) & 3];
}

void ElementSpinningGunTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t sx = stat->x;
	uint8_t sy = stat->y;

	board_draw_tile(sx, sy);

	int8_t dx, dy;
	uint8_t element = (stat->p2 & 0x80) ? E_STAR : E_BULLET;

	if (RAND9() < (stat->p2 & 0x7F)) {
		if (RAND9() <= stat->p1) {
			bool shot;

			if (difference8(sx, ZOO_STAT(0).x) <= 2) {
				shot = board_shoot(element, sx, sy,
					0, signum8(ZOO_STAT(0).y - sy),
					SHOT_SOURCE_ENEMY);
			} else {
				shot = false;
			}

			if (!shot) {
				if (difference8(sy, ZOO_STAT(0).y) <= 2) {
					shot = board_shoot(element, sx, sy,
						signum8(ZOO_STAT(0).x - sx), 0,
						SHOT_SOURCE_ENEMY);
				}
			}
		} else {
			calc_direction_rnd(&dx, &dy);
			board_shoot(element, sx, sy, dx, dy, SHOT_SOURCE_ENEMY);
		}
	}
}

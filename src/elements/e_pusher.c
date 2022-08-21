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

uint8_t ElementPusherDraw(uint8_t x, uint8_t y) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->step_x == 1) {
		return 16;
	} else if (stat->step_x == -1) {
		return 17;
	} else if (stat->step_y == -1) {
		return 30;
	} else {
		return 31;
	}
}

void ElementPusherTick(uint8_t stat_id) {
	uint8_t i, start_x, start_y;
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	start_x = stat->x;
	start_y = stat->y;

	if (!(zoo_element_defs_flags[ZOO_TILE(start_x + stat->step_x, start_y + stat->step_y).element] & ELEMENT_WALKABLE)) {
		ElementPushablePush(start_x + stat->step_x, start_y + stat->step_y, stat->step_x, stat->step_y);
	}
	
	stat_id = get_stat_id_at(start_x, start_y);
	stat = &ZOO_STAT(stat_id);

	if (zoo_element_defs_flags[ZOO_TILE(start_x + stat->step_x, start_y + stat->step_y).element] & ELEMENT_WALKABLE) {
		move_stat(stat_id, start_x + stat->step_x, start_y + stat->step_y);
		sound_queue(2, sound_push);

		uint8_t nx = stat->x - (stat->step_x * 2);
		uint8_t ny = stat->y - (stat->step_y * 2);
		if (ZOO_TILE(nx, ny).element == E_PUSHER) {
			i = get_stat_id_at(nx, ny);
			zoo_stat_t *nstat = &ZOO_STAT(i);

			if (nstat->step_x == stat->step_x && nstat->step_y == stat->step_y) {
				ElementPusherTick(i);
			}
		}
	}
}

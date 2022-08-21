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
#include "../../res/message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

uint8_t ElementBombDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 <= 1) return 11;
	else return 48 + p1;
}

void ElementBombTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->p1 > 0) {
		uint8_t p1 = --stat->p1;
		board_draw_tile(stat->x, stat->y);

		if (p1 == 1) {
			sound_queue(1, sound_bomb_explosion);
			DrawPlayerSurroundings(stat->x, stat->y, 1);
		} else if (p1 == 0) {
			uint8_t old_x = stat->x;
			uint8_t old_y = stat->y;
			remove_stat(stat_id);
			DrawPlayerSurroundings(old_x, old_y, 2);
		} else if ((p1 & 1) == 0) {
			sound_queue(1, sound_bomb_tick1);
		} else {
			sound_queue(1, sound_bomb_tick2);
		}
	}
}

void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->p1 == 0) {
		stat->p1 = 9;
		board_draw_tile(x, y);
		display_message(200, NULL, NULL, msg_bomb_activated);
		sound_queue(4, sound_bomb_activated);
	} else {
		ElementPushablePush(x, y, *dx, *dy);
	}
}

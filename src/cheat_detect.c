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
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "sound_consts.h"
#include "timer.h"

#ifdef SHOW_CHEATS

static uint8_t cheat_update_table[] = {
	J_UP, 0, J_UP,
	0, J_DOWN, 0, J_DOWN,
	0, J_LEFT, 0, J_RIGHT,
	0, J_LEFT, 0, J_RIGHT,
	0, J_B, 0, J_A
};
#define CHEAT_UPDATE_TABLE_LEN 19

void cheat_detect_update(void) BANKED {
	if (cheat_active == 255) return;

	if (input_held == cheat_update_table[cheat_active]) {
		cheat_active++;
		if (cheat_active >= CHEAT_UPDATE_TABLE_LEN) {
			cheat_active = 255;
			sound_queue(10, sound_cheat);
		}
	}
}

#endif

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

#include "game.h"

#include <gbdk/platform.h>
#include "bank_switch.h"
#include "config.h"
#include "game_transition.h"

#ifdef FEAT_BOARD_TRANSITIONS

// #define TRANSITION_TABLE_20_17_ENTRY_COUNT 340
// extern const uint8_t transition_table_20_17[];
#define TRANSITION_TABLE_20_18_ENTRY_COUNT 360
extern const uint8_t transition_table_20_18[];

// must be a divisor of the table size

static uint16_t transition_pos = 0xFFFF;
static uint8_t transition_color;

extern int8_t viewport_x;
extern int8_t viewport_y;

void game_transition_start(uint8_t color) {
	while (transition_pos < (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2)) {
__asm
		halt
__endasm;
	}
	transition_pos = 0;
	transition_color = color;
}

void game_transition_step(void) {
	if (transition_pos >= (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2)) return;

	const uint8_t *arr = transition_table_20_18 + transition_pos;
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	if (transition_color == 0xFF) {
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		transition_pos += 12;
	} else {
		for (uint8_t i = 0; i < 7; i++) {
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			text_draw(x, y, 178, transition_color);
		}
		transition_pos += 14;
	}

	ZOO_SWITCH_ROM(prev_bank);
}

bool game_transition_running(void) {
	return transition_pos < (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2);
}

#endif

/* void game_transition_tick_to_end(void) BANKED {
	while (transition_pos != TRANSITION_TABLE_20_18_ENTRY_COUNT) {
		game_transition_step();
	}
} */

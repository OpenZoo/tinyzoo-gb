#pragma bank 3
// Must be in the same bank as the "small" font and message_consts.

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
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>
#include "../gamevars.h"
#include "../math.h"
#include "../renderer_sidebar.h"
#include "../../res/font_small.h"

extern uint8_t sidebar_tile_data_ly_switch;
extern uint8_t sidebar_tile_data[96];

static void sidebar_draw_char(uint8_t x, uint8_t chr, uint8_t col) {
	const uint8_t *font_data = _font_small_bin + (chr << 3);
	uint8_t mask = (x & 1) ? 0x0F : 0xF0;
	uint8_t i;

	uint8_t *tile_data = sidebar_tile_data + ((x << 3) & 0xF0);

	if (col == 3) {
		for (i = 0; i < 8; i++) {
			uint8_t f = font_data[i] & mask;
			*(tile_data++) |= f;
			*(tile_data++) |= f;
		}
	} else {
		if (col == 2) tile_data++;
		for (i = 0; i < 8; i++) {
			uint8_t f = font_data[i] & mask;
			*(tile_data++) |= f;
			tile_data++;
		}
	}
}

void sidebar_copy_data(uint16_t address, uint8_t len);

// TODO: Fall back gracefully on DMG
void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED {
	uint8_t text[5];
	uint8_t data_len = wide ? 64 : 48;

	memset(&sidebar_tile_data, 0, data_len);
	sidebar_draw_char(0, chr, col);
	uint8_t offset = 2;

	if (wide) {
DrawFullNumber:
		// six tiles
		if (value < 0) {
			sidebar_draw_char(offset++, '-', 3);
			value = -value;
		}
		if (value == 0) {
			sidebar_draw_char(offset, '0', 3);
		} else {
			uint8_t text_pos = 5;
			while (value > 0) {
				text[--text_pos] = 48 + (value % 10);
				value /= 10;
			}
			while (text_pos < 5) {
				sidebar_draw_char(offset++, text[text_pos++], 3);
			}
		}
	} else {
		if (value <= -100) {
			// TODO?
			sidebar_draw_char(offset++, '?', 3);
		} else if (value >= 10000) {
			sidebar_draw_char(offset++, (value / 10000) + 48, 3);
			sidebar_draw_char(offset++, zoo_mods16_8((value / 1000), 10) + 48, 3);
			sidebar_draw_char(offset++, 'K', 3);
		} else if (value >= 1000) {
			sidebar_draw_char(offset++, (value / 1000) + 48, 3);
			sidebar_draw_char(offset++, 'K', 3);
			if (value > 0) {
				sidebar_draw_char(offset++, '+', 3);
			}
		} else goto DrawFullNumber;
	}

	sidebar_copy_data(0x9000 | (x_shifted << 4), data_len);
}

// TODO: Fall back gracefully on DMG
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED {
	memset(&sidebar_tile_data, 0, sizeof(sidebar_tile_data));

	if (value & 0x02) sidebar_draw_char(1, 0x0C, 1);
	if (value & 0x04) sidebar_draw_char(2, 0x0C, 2);
	if (value & 0x08) sidebar_draw_char(3, 0x0C, 3);
	if (value & 0x10) sidebar_draw_char(4, 0x0C, 1);
	if (value & 0x20) sidebar_draw_char(5, 0x0C, 2);
	if (value & 0x40) sidebar_draw_char(6, 0x0C, 2);
	if (value & 0x80) sidebar_draw_char(7, 0x0C, 3);

	sidebar_copy_data(0x9000 | (x_shifted << 4), 64);
}

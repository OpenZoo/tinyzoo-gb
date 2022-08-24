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

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "renderer.h"
#include "renderer_sidebar.h"

AT(0xC040) uint8_t sidebar_tile_data[96];
extern uint8_t sidebar_tile_data_ly_switch;

extern uint8_t ly_bank_switch;
extern uint8_t ly_offset;

void sidebar_copy_data(uint16_t address, uint8_t len) {
	safe_vmemcpy((uint8_t*) address, sidebar_tile_data, len >> 1);
}

static uint8_t sb_offset;

static void sidebar_show_line(const char *line1) {
	if (line1 != NULL) {
		uint8_t slen = *(line1++);
		if (slen > 0) {
			memset(sidebar_tile_data + sb_offset, 0, 20);
			memcpy(sidebar_tile_data + sb_offset + (10 - (slen >> 1)), line1, slen);
			sb_offset += 32;
		}
	}
}

void sidebar_show_message(const char* line1, uint8_t bank1, const char* line2, uint8_t bank2, const char* line3, uint8_t bank3) {
	sb_offset = 0;

	// write text data
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(bank1);
	sidebar_show_line(line1);
	ZOO_SWITCH_ROM(bank2);
	sidebar_show_line(line2);
	ZOO_SWITCH_ROM(bank3);
	sidebar_show_line(line3);
	ZOO_SWITCH_ROM(prev_bank);

	sidebar_tile_data_ly_switch = ly_offset - (sb_offset >> 2);
#ifdef HACK_HIDE_STATUSBAR
	if (1) {
#else
	if (renderer_mode == RENDER_MODE_TITLE) {
#endif
		sidebar_copy_data(0x9C00 + (15 << 5) + (96 - sb_offset), sb_offset);
	} else {
		sidebar_copy_data(0x9C00 + (14 << 5) + (96 - sb_offset), sb_offset);
	}
}

void sidebar_hide_message(void) {
	ly_bank_switch = ly_offset;
}

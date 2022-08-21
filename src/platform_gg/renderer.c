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
#include "gamevars.h"
#include "renderer.h"
#include "../../res/font_default.h"

#define COL_LVL_0 0
#define COL_LVL_1 5
#define COL_LVL_2 10
#define COL_LVL_3 15

const uint16_t gg_palette[16] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_1, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3)
};

uint8_t renderer_scrolling;

void text_init(uint8_t mode) {
	// initialize GG palette
	set_palette(0, 1, gg_palette);

	volatile uint16_t address = 0x3800 + ((3 * 32 + 6) << 1);
	uint16_t i = 0;
	for (uint8_t iy = 0; iy < 18; iy++) {
		for (uint8_t ix = 0; ix < 20; ix++, i++) {
			set_vram_byte((uint8_t*) (address), (i >> 8));
			address++;
			set_vram_byte((uint8_t*) (address), i);
			address++;
		}
		address += 12 << 1;
	}

	// TODO

	__WRITE_VDP_REG(VDP_R0, 0x16);
	__WRITE_VDP_REG(VDP_R1, R1_DEFAULT | R1_DISP_ON | R1_IE)
	__WRITE_VDP_REG(VDP_R2, R2_MAP_0x3800);
}

void text_sync_hblank_safe(void) {
	// TODO
}

void text_undraw(uint8_t x, uint8_t y) {

}

void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	volatile uint16_t vram_address = ((y * 20) + x) * 32;
	uint16_t char_address = chr * 8;
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	for (uint8_t iy = 0; iy < 8; iy++, char_address++) {
		set_vram_byte(vram_address, _font_default_bin[char_address]);
		vram_address++;
		set_vram_byte(vram_address, _font_default_bin[char_address]);
		vram_address++;
		set_vram_byte(vram_address, _font_default_bin[char_address]);
		vram_address++;
		set_vram_byte(vram_address, _font_default_bin[char_address]);
		vram_address++;
	}

	ZOO_SWITCH_ROM(prev_bank);
}

void text_free_line(uint8_t y) {

}

void text_scroll(int8_t dx, int8_t dy) {
	// TODO
}

void text_update(void) {
	// TODO
}

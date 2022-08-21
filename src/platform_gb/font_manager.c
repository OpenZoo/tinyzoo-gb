#pragma bank 3

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

#include <stdint.h>
#include <string.h>
#include "font_manager.h"
#include "../../res/font_default.h"

void font_8x8_install(uint8_t color_bg, uint8_t color_fg) BANKED {
	// TODO: Rewrite to ASM, optimize

	uint8_t mask_bg_1 = ((color_bg & 1) ? 0xFF : 0x00);
	uint8_t mask_bg_2 = ((color_bg & 2) ? 0xFF : 0x00);
	uint8_t mask_fg_1 = ((color_fg & 1) ? 0xFF : 0x00);
	uint8_t mask_fg_2 = ((color_fg & 2) ? 0xFF : 0x00);

	volatile uint8_t *vptr = (uint8_t*) 0x8000;
	const uint8_t *fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		uint8_t vf = *fptr;
		uint8_t v1 = (mask_bg_1 & (~vf)) | (mask_fg_1 & (vf));
		uint8_t v2 = (mask_bg_2 & (~vf)) | (mask_fg_2 & (vf));
		*(vptr++) = v1;
		*(vptr++) = v2;
	}
}

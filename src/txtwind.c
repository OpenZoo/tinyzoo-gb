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

#include <gbdk/platform.h>
#include <string.h>

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "sram_alloc.h"
#include "txtwind.h"

void txtwind_read_line(int16_t idx, txtwind_line_t *line) {
	if (idx < 0 || idx >= ((int16_t) txtwind_lines)) {
		line->type = TXTWIND_LINE_TYPE_REGULAR;
		line->len = 0;
		return;
	}

	sram_ptr_t ptr;
	far_ptr_t fptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (idx * 3);

	ZOO_ENABLE_RAM;
	sram_read(&ptr, &fptr, 3);
	ZOO_DISABLE_RAM;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(fptr.bank);
	const txtwind_line_t *tptr = (const txtwind_line_t*) fptr.ptr;
	memcpy(line, tptr, TXTWIND_LINE_HEADER_LEN + tptr->len + 2 /* hyperlink */);
	ZOO_SWITCH_ROM(prev_bank);
}

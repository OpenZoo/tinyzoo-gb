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

#ifndef __TXTWIND_H__
#define __TXTWIND_H__

#include <stdbool.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include "gamevars.h"

#define TXTWIND_LINE_TYPE_REGULAR 0
#define TXTWIND_LINE_TYPE_CENTERED 1
#define TXTWIND_LINE_TYPE_HYPERLINK 2
#define MAX_TXTWIND_LINE_WIDTH 20
#define TXTWIND_LINE_HEADER_LEN 2
#define TXTWIND_LINE_AFTER_LEN 2

typedef struct {
	uint8_t type;
	uint8_t len;
	uint8_t text[MAX_TXTWIND_LINE_WIDTH + TXTWIND_LINE_AFTER_LEN];
} txtwind_line_t;

extern uint16_t txtwind_lines;

// txtwind.c (bank 0)

void txtwind_read_line(int16_t idx, txtwind_line_t *line);

// txtwind_bank2.c (bank 2)

void txtwind_init(void) BANKED;
void txtwind_append(uint16_t line_ptr, uint8_t line_bank) BANKED;
void txtwind_open_license(void) BANKED;
uint8_t txtwind_run(uint8_t render_mode) BANKED;

#endif /* __TXTWIND_H__ */

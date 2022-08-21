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

#ifndef __SRAM_ALLOC_H__
#define __SRAM_ALLOC_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>
#include "config.h"
#include "gamevars.h"

typedef struct {
	uint8_t bank;
	int16_t position;
} sram_ptr_t;

#define SRAM_DATA_POS 4
#define SRAM_BOARD_PTRS_POS (SRAM_DATA_POS + 2 + sizeof(zoo_world_info_t))
#define SRAM_TEXT_WINDOW_POS (SRAM_BOARD_PTRS_POS + (MAX_BOARD * sizeof(sram_ptr_t)))

typedef struct {
	uint8_t magic[4];
	uint8_t flags;
	uint8_t world_id;
	zoo_world_info_t world_info;
	sram_ptr_t board_pointers[MAX_BOARD];
	far_ptr_t text_window_lines[MAX_TEXT_WINDOW_LINES];
} sram_header_t;

// Pre-setup required: ENABLE_RAM, switch bank
void sram_add_ptr(sram_ptr_t *ptr, uint16_t val);
void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val);

#if defined(DEBUG_SRAM_WRITES) && !defined(SRAM_ALLOC_INTERNAL)
uint8_t sram_read8_debug(sram_ptr_t *ptr);
void sram_write8_debug(sram_ptr_t *ptr, uint8_t value);
void sram_read_debug(sram_ptr_t *ptr, void *data, uint16_t len);
void sram_write_debug(sram_ptr_t *ptr, const void *data, uint16_t len);
#endif

#if defined(DEBUG_SRAM_WRITES) && !defined(SRAM_ALLOC_INTERNAL)
#define sram_read8 sram_read8_debug
#define sram_write8 sram_write8_debug
#else
uint8_t sram_read8(sram_ptr_t *ptr);
void sram_write8(sram_ptr_t *ptr, uint8_t value);
#endif

#if defined(SRAM_ALLOC_INTERNAL)
void sram_read(sram_ptr_t *ptr, uint8_t *data, uint16_t len);
void sram_write(sram_ptr_t *ptr, const uint8_t *data, uint16_t len);
#else
#if defined(DEBUG_SRAM_WRITES)
#define sram_read sram_read_debug
#define sram_write sram_write_debug
#else
void sram_read(sram_ptr_t *ptr, void *data, uint16_t len);
void sram_write(sram_ptr_t *ptr, const void *data, uint16_t len);
#endif
#endif

bool sram_alloc(uint16_t len, sram_ptr_t *ptr);
void sram_free(sram_ptr_t *ptr);
void sram_toggle_write(void);

// No pre-setup required.
void sram_init(bool force) BANKED;

#endif

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

#include <string.h>
#include <gbdk/platform.h>
#include "gamevars.h"
#include "game.h"
#include "bank_switch.h"
#include "board_manager.h"
#include "config.h"

static uint8_t board_pointers_bank;
static far_ptr_t *board_pointers_ptr;

void load_world_rom(uint8_t offset) NONBANKED {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(ROM_DATA_START_BANK);

	far_ptr_t *world_ptr_ptr = (far_ptr_t*) (0x4000 | (offset * 3));
	uint8_t *data = world_ptr_ptr->ptr;
	board_pointers_bank = world_ptr_ptr->bank;
	ZOO_SWITCH_ROM(board_pointers_bank);

	// load world info
	memcpy(&zoo_world_info, data, sizeof(zoo_world_info_t));
	data += sizeof(zoo_world_info_t);
	board_pointers_ptr = (far_ptr_t*) data;

	ZOO_SWITCH_ROM(prev_bank);
}

void load_board_rom(uint8_t offset) NONBANKED {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(board_pointers_bank);

	far_ptr_t *ptr = &board_pointers_ptr[offset];
	load_board_data_rom(ptr->bank, ptr->ptr);

	ZOO_SWITCH_ROM(prev_bank);
}

void load_board_data_rom(uint8_t bank, const uint8_t *data) NONBANKED {
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(bank);

	// RLE decode
	uint8_t ix = 1;
	uint8_t iy = 1;
	uint8_t rle_count = 0;
	zoo_tile_t rle_tile;
	while (true) {
		if (rle_count <= 0) {
				rle_tile.element = *(data++);
				rle_tile.color = *(data++);
				if (rle_tile.element & 0x80) {
					rle_count = 1;
					rle_tile.element &= 0x7F;
				} else {
					rle_count = *(data++);
				}
		}
		ZOO_TILE_COPY(ZOO_TILE(ix, iy), rle_tile);
		ix++;
		if (ix > BOARD_WIDTH) {
				ix = 1;
				iy++;
				if (iy > BOARD_HEIGHT) break;
		}
		rle_count--;
	}

	memcpy(&zoo_board_info, data, sizeof(zoo_board_info_t));
	data += sizeof(zoo_board_info_t);

	zoo_stat_count = *(data++);

	uint16_t zoo_stat_size = sizeof(zoo_stat_t) * (zoo_stat_count + 1);
	memcpy(zoo_stats + 1, data, zoo_stat_size);
	data += zoo_stat_size;

	zoo_stat_data_size = *((uint16_t*)data);
	data += 2;
	memcpy(zoo_stat_data, data, zoo_stat_data_size);

	ZOO_SWITCH_ROM(prev_bank);
}

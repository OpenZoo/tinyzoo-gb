#pragma bank 2

#include <string.h>
#include <gb/gb.h>
#include "gamevars.h"
#include "game.h"
#include "board_manager.h"
#include "config.h"
#include "sram_alloc.h"

#define SRAM_DATA_POS 4
#define SRAM_BOARD_PTRS_POS (SRAM_DATA_POS + 2 + sizeof(zoo_world_info_t))

static bool load_world_sram(uint8_t offset) {
	sram_ptr_t ptr;
	uint8_t result[2];

	ptr.bank = 0;
	ptr.position = SRAM_DATA_POS;
	sram_read(&ptr, result, 2);
	if (!(result[0] & 0x01) || result[1] != offset) {
		return false;
	}

	sram_read(&ptr, &zoo_world_info, sizeof(zoo_world_info_t));

	return true;
}

static void save_world_sram(uint8_t offset, bool force) {
	sram_ptr_t ptr;
	uint8_t result[2];
	
	ptr.bank = 0;
	ptr.position = SRAM_DATA_POS;
	sram_read(&ptr, result, 2);
	result[0] |= 0x01;
	if (offset != 0xFF) result[1] = offset;
	sram_sub_ptr(&ptr, 2);
	sram_write(&ptr, result, 2);
	
	sram_write(&ptr, &zoo_world_info, sizeof(zoo_world_info_t));

	if (force) {
		// clear pointer area
		for (uint8_t i = 0; i < MAX_BOARD; i++) {
			sram_write8(&ptr, 0);
			sram_write8(&ptr, 0);
			sram_write8(&ptr, 0);
		}
	}
}

static void load_board_data_sram(sram_ptr_t *the_ptr) {
	sram_ptr_t ptr;
	ptr.bank = the_ptr->bank;
	ptr.position = the_ptr->position;

	// RLE decode
	uint8_t ix = 1;
	uint8_t iy = 1;
	uint8_t rle_count = 0;
	zoo_tile_t rle_tile;
	while (true) {
		if (rle_count <= 0) {
				rle_tile.element = sram_read8(&ptr);
				rle_tile.color = sram_read8(&ptr);
				if (rle_tile.element & 0x80) {
					rle_count = 1;
					rle_tile.element &= 0x7F;
				} else {
					rle_count = sram_read8(&ptr);
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

	sram_read(&ptr, &zoo_board_info, sizeof(zoo_board_info_t));
	zoo_stat_count = sram_read8(&ptr);
	sram_read(&ptr, (zoo_stats + 1), sizeof(zoo_stat_t) * (zoo_stat_count + 1));
}

static bool save_board_data_sram(sram_ptr_t *new_ptr) {
	// calculate board size
	uint16_t board_size = (zoo_stat_count + 1) * sizeof(zoo_stat_t) + 1 + sizeof(zoo_board_info_t);

	// RLE encode (first pass)
	uint8_t ix = 1;
	uint8_t iy = 1;
	uint8_t rle_count = 1;
	zoo_tile_t rle_tile, curr_tile;
	ZOO_TILE_COPY(rle_tile, ZOO_TILE(ix, iy));
	while (iy <= BOARD_HEIGHT) {
		ix++;
		if (ix > BOARD_WIDTH) {
				ix = 1;
				iy++;
		}
		ZOO_TILE_COPY(curr_tile, ZOO_TILE(ix, iy));
		if (*((uint16_t*) &rle_tile) == *((uint16_t*) &curr_tile) && rle_count < 255 && iy <= BOARD_HEIGHT) {
			rle_count++;
		} else {
			board_size += (rle_count > 1) ? 3 : 2;
			ZOO_TILE_COPY(rle_tile, curr_tile);
			rle_count = 1;
		}
	}

	// allocate
	if (!sram_alloc(board_size, new_ptr)) {
		return false;
	}

	sram_ptr_t ptr;
	ptr.bank = new_ptr->bank;
	ptr.position = new_ptr->position;

	// RLE encode (second pass)
	ix = 1;
	iy = 1;
	rle_count = 1;
	ZOO_TILE_COPY(rle_tile, ZOO_TILE(ix, iy));
	while (iy <= BOARD_HEIGHT) {
		ix++;
		if (ix > BOARD_WIDTH) {
				ix = 1;
				iy++;
		}
		ZOO_TILE_COPY(curr_tile, ZOO_TILE(ix, iy));
		if (*((uint16_t*) &rle_tile) == *((uint16_t*) &curr_tile) && rle_count < 255 && iy <= BOARD_HEIGHT) {
			rle_count++;
		} else {
			if (rle_count == 1) {
				sram_write8(&ptr, rle_tile.element | 0x80);
				sram_write8(&ptr, rle_tile.color);
			} else {
				sram_write8(&ptr, rle_tile.element);
				sram_write8(&ptr, rle_tile.color);
				sram_write8(&ptr, rle_count);
			}
			ZOO_TILE_COPY(rle_tile, curr_tile);
			rle_count = 1;
		}
	}

	sram_write(&ptr, &zoo_board_info, sizeof(zoo_board_info_t));
	sram_write8(&ptr, zoo_stat_count);
	sram_write(&ptr, (zoo_stats + 1), sizeof(zoo_stat_t) * (zoo_stat_count + 1));

	return true;
}

// on bank 2 - BANKED

void load_world(uint8_t offset) BANKED {
	load_world_rom(offset); // initializes pointers to ROM

	ENABLE_RAM_MBC5;

	if (!load_world_sram(offset)) {
		save_world_sram(offset, true);
	}

	DISABLE_RAM_MBC5;
}

void load_board(uint8_t offset) BANKED {
	sram_ptr_t ptr, board_ptr;

	ptr.bank = 0;
	ptr.position = SRAM_BOARD_PTRS_POS + (offset * sizeof(sram_ptr_t));

	ENABLE_RAM_MBC5;

	sram_read(&ptr, &board_ptr, sizeof(sram_ptr_t));
	if (board_ptr.bank != 0 || board_ptr.position != 0) {
		load_board_data_sram(&board_ptr);
		DISABLE_RAM_MBC5;
	} else {
		DISABLE_RAM_MBC5;
		load_board_rom(offset);
	}
}

bool save_board(uint8_t offset) BANKED {
	sram_ptr_t ptr, board_ptr;

	ptr.bank = 0;
	ptr.position = SRAM_BOARD_PTRS_POS + (offset * sizeof(sram_ptr_t));

	ENABLE_RAM_MBC5;

	sram_read(&ptr, &board_ptr, sizeof(sram_ptr_t));
	sram_sub_ptr(&ptr, sizeof(sram_ptr_t));
	if (board_ptr.bank != 0 || board_ptr.position != 0) {
		sram_free(&board_ptr);
	}

	bool result = save_board_data_sram(&board_ptr);
	if (!result) {
		board_ptr.bank = 0;
		board_ptr.position = 0;
	}
	sram_write(&ptr, &board_ptr, sizeof(sram_ptr_t));

	return result;
}
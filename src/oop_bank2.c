#include "sram_debug.h"
#pragma bank 2

#include <string.h>
#include <gb/gb.h>
#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "gamevars.h"
#include "config.h"
#include "math.h"
#include "oop.h"
#include "sram_alloc.h"
#include "sram_debug.h"
#include "txtwind.h"

#include "element_defs_cycles.inc"

void oop_banked_noop_why(void) BANKED {
	
}

static uint8_t get_color_for_tile_match(uint8_t element, uint8_t color) {
	uint8_t def_color = zoo_element_defs_color[element];
	if (def_color < COLOR_SPECIAL_MIN) {
		return def_color & 0x07;
	} else if (def_color == COLOR_WHITE_ON_CHOICE) {
		return ((color >> 4) & 0x0F) | 8;
	} else {
		return color & 0x0F;
	}
}

// TODO: This method is *SLOW*. Really, really *SLOW*.
bool find_tile_on_board(uint8_t *x, uint8_t *y, uint8_t element, uint8_t color) BANKED {
	zoo_tile_t tile;

	while (true) {
		if ((++(*x)) > BOARD_WIDTH) {
			*x = 1;
			if ((++(*y)) > BOARD_HEIGHT) {
				return false;
			}
		}

		ZOO_TILE_ASSIGN(tile, (*x), (*y));
		if (tile.element == element) {
			if (tile.color == 0 || get_color_for_tile_match(tile.element, tile.color) == color) {
				return true;
			}
		}
	}
}

void oop_place_tile(uint8_t x, uint8_t y, uint8_t element, uint8_t color) BANKED {
	zoo_tile_t *src_tile = &ZOO_TILE(x, y);

	if (src_tile->element != E_PLAYER) {
		uint8_t new_color = color;
		uint8_t def_color = zoo_element_defs_color[element];
		if (def_color < COLOR_SPECIAL_MIN) {
			new_color = def_color;
		} else {
			if (new_color == 0) {
				new_color = src_tile->color;
			}

			if (new_color == 0) {
				new_color = 0x0F;
			}

			if (def_color == COLOR_WHITE_ON_CHOICE) {
				new_color = ((new_color - 8) << 4) | 0x0F;
			}
		}

		if (src_tile->element == element) {
			src_tile->color = new_color;
		} else {
			board_damage_tile(x, y);
			if (zoo_element_defs_flags[element] & ELEMENT_TYPICALLY_STATTED) {
				add_stat(x, y, element, new_color, zoo_element_defs_cycles[element], &stat_template_default);
			} else {
				src_tile->element = element;
				src_tile->color = new_color;
			}
		}

		board_draw_tile(x, y);
	}
}

uint16_t oop_dataofs_clone(uint16_t loc) BANKED {
	uint8_t len = zoo_stat_data[loc + 3] & 0x7F;
	memcpy(zoo_stat_data + zoo_stat_data_size, zoo_stat_data + loc, len);
	uint16_t new_pos = zoo_stat_data_size;
	zoo_stat_data_size += len;
	return new_pos;
}

void oop_dataofs_free_if_unused(uint16_t loc) BANKED {
	zoo_stat_t *stat = &ZOO_STAT(0);
	uint8_t stat_id = 0;
	for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
		if (stat->data_ofs == loc) {
			return;
		}
	}
	oop_dataofs_free(loc);
}

void oop_dataofs_free(uint16_t loc) BANKED {
	uint8_t len = zoo_stat_data[loc + 3] & 0x7F;
	memmove(zoo_stat_data + loc + len, zoo_stat_data + loc, zoo_stat_data_size - loc - len);
	zoo_stat_data_size -= len;

	zoo_stat_t *stat = &ZOO_STAT(0);
	uint8_t stat_id = 0;
	for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
		if (stat->data_ofs != 0xFFFF) {
			if (stat->data_ofs > loc) {
				stat->data_ofs -= len;
			}
		}
	}
}

extern uint16_t oop_window_zzt_lines;

void oop_handle_txtwind(void) BANKED {
	// TODO
	/* if (oop_window_zzt_lines > 1) {
		
	} else if (oop_window_zzt_lines == 1) */ {
		uint8_t sram_ptr_data[9];
		sram_ptr_t sram_ptr;

		ENABLE_RAM_MBC5;
		sram_ptr.bank = 0;
		sram_ptr.position = SRAM_TEXT_WINDOW_POS;
		sram_read(&sram_ptr, sram_ptr_data, sizeof(sram_ptr_data));
		DISABLE_RAM_MBC5;

		// TODO: This doesn't handle 1-liners prefixed with $ or !.
		// Let's hope they don't actually occur much?

		switch (txtwind_lines) {
			case 1:
				display_message_nobank(200, NULL, 3, NULL, 3,
					(const char*) (*((uint16_t**) (sram_ptr_data))) + 3, sram_ptr_data[2]);
				break;
			case 2:
				display_message_nobank(200, NULL, 3,
					(const char*) (*((uint16_t**) (sram_ptr_data))) + 3, sram_ptr_data[2],
					(const char*) (*((uint16_t**) (sram_ptr_data + 3))) + 3, sram_ptr_data[5]);
				break;
			default:
				display_message_nobank(200,
					(const char*) (*((uint16_t**) (sram_ptr_data))) + 3, sram_ptr_data[2],
					(const char*) (*((uint16_t**) (sram_ptr_data + 3))) + 3, sram_ptr_data[5],
					(const char*) (*((uint16_t**) (sram_ptr_data + 6))) + 3, sram_ptr_data[8]);
				break;
		}
	}
}
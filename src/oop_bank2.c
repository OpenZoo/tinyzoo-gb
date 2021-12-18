#include "stdbool.h"
#pragma bank 2

#include <gb/gb.h>
#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "gamevars.h"
#include "config.h"
#include "math.h"
#include "oop.h"

#include "element_defs_cycles.inc"

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

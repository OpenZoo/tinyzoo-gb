#pragma bank 3
// Must be in the same bank as the "small" font.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer_sidebar.h"
#include "../res/font_small.h"

extern uint8_t sidebar_tile_data_len;
extern uint16_t sidebar_tile_data_address;
extern uint8_t sidebar_tile_data[64];
extern volatile bool sidebar_tile_data_awaiting;

static void sidebar_draw_char(uint8_t x, uint8_t chr, uint8_t col) {
	const uint8_t *font_data = _font_small_bin + (chr << 3);
	uint8_t mask = (x & 1) ? 0x0F : 0xF0;
	uint8_t i;

	uint8_t *tile_data = sidebar_tile_data + ((x << 3) & 0xF0);

	for (i = 0; i < 8; i++) {
		uint8_t f = font_data[i];
		if (col & 1) {
			tile_data[i << 1] |= (f & mask);
		}
		if (col & 2) {
			tile_data[(i << 1) | 1] |= (f & mask);
		}
	}
}


static void sidebar_copy_data(uint16_t address, uint8_t len) {
	sidebar_tile_data_address = address;
	sidebar_tile_data_len = len;
	sidebar_tile_data_awaiting = true;
}

// TODO: Fall back gracefully on DMG
void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED {
	uint8_t text[5];
	uint8_t text_pos = 5;

	while (sidebar_tile_data_awaiting) {}

	memset(&sidebar_tile_data, 0, sizeof(sidebar_tile_data));
	sidebar_draw_char(0, chr, col);
	uint8_t offset = 2;

	if (wide || ((value >= -99) && (value <= 999))) {
		// six tiles
		if (value < 0) {
			sidebar_draw_char(offset++, '-', 3);
			value = -value;
		}
		if (value == 0) {
			sidebar_draw_char(offset, '0', 3);
		} else {
			while (value > 0) {
				text[--text_pos] = 48 + (value % 10);
				value /= 10;
			}
			while (text_pos < 5) {
				sidebar_draw_char(offset++, text[text_pos++], 3);
			}
		}
	} else {
		if (value <= -100) {
			// TODO?
			sidebar_draw_char(offset++, '?', 3);
		} else if (value >= 10000) {
			sidebar_draw_char(offset++, (value / 10000), 3);
			sidebar_draw_char(offset++, (value / 1000) % 10, 3);
			sidebar_draw_char(offset++, 'K', 3);
		} else if (value >= 1000) {
			sidebar_draw_char(offset++, (value / 1000), 3);
			sidebar_draw_char(offset++, 'K', 3);
			if (value > 0) {
				sidebar_draw_char(offset++, '+', 3);
			}
		}
	}

	sidebar_copy_data(0x9000 | (x_shifted << 4), wide ? 64 : 48);
}

// TODO: Fall back gracefully on DMG
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED {
	while (sidebar_tile_data_awaiting) {}

	memset(&sidebar_tile_data, 0, sizeof(sidebar_tile_data));

	if (value & 0x02) sidebar_draw_char(1, 0x0C, 1);
	if (value & 0x04) sidebar_draw_char(2, 0x0C, 2);
	if (value & 0x08) sidebar_draw_char(3, 0x0C, 3);
	if (value & 0x10) sidebar_draw_char(4, 0x0C, 1);
	if (value & 0x20) sidebar_draw_char(5, 0x0C, 2);
	if (value & 0x40) sidebar_draw_char(6, 0x0C, 2);
	if (value & 0x80) sidebar_draw_char(7, 0x0C, 3);

	sidebar_copy_data(0x9000 | (x_shifted << 4), 64);
}
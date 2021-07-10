#pragma bank 3
// Must be in the same bank as the "small" font.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer_sidebar.h"
#include "../res/font_small.h"

static void sidebar_draw_char(uint8_t *tile_data, uint8_t x, uint8_t chr, uint8_t col) {
	const uint8_t *font_data = _font_small_bin + (chr << 3);
	uint8_t mask = (x & 1) ? 0x0F : 0xF0;
	uint8_t i;

	tile_data += ((x << 3) & 0xF0);

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

static void sidebar_copy_data(uint16_t address, const uint8_t *data, uint8_t len) BANKED {
	// TODO: Use hblank
	wait_vbl_done();
	memcpy((uint8_t*) address, data, len);
}

// TODO: Fall back gracefully on DMG
void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED {
	uint8_t text[5];
	uint8_t text_pos = 5;
	uint8_t tile_data[64];

	memset(&tile_data, 0, sizeof(tile_data));
	sidebar_draw_char(tile_data, 0, chr, col);
	uint8_t offset = 2;

	if (wide || ((value >= -99) && (value <= 999))) {
		// six tiles
		if (value < 0) {
			sidebar_draw_char(tile_data, offset++, '-', 3);
			value = -value;
		}
		if (value == 0) {
			sidebar_draw_char(tile_data, offset, '0', 3);
		} else {
			while (value > 0) {
				text[--text_pos] = 48 + (value % 10);
				value /= 10;
			}
			while (text_pos < 5) {
				sidebar_draw_char(tile_data, offset++, text[text_pos++], 3);
			}
		}
	} else {
		if (value <= -100) {
			// TODO?
			sidebar_draw_char(tile_data, offset++, '?', 3);
		} else if (value >= 10000) {
			sidebar_draw_char(tile_data, offset++, (value / 10000), 3);
			sidebar_draw_char(tile_data, offset++, (value / 1000) % 10, 3);
			sidebar_draw_char(tile_data, offset++, 'K', 3);
		} else if (value >= 1000) {
			sidebar_draw_char(tile_data, offset++, (value / 1000), 3);
			sidebar_draw_char(tile_data, offset++, 'K', 3);
			if (value > 0) {
				sidebar_draw_char(tile_data, offset++, '+', 3);
			}
		}
	}

	sidebar_copy_data(0x9000 | (x_shifted << 4), tile_data, wide ? 64 : 48);
}

// TODO: Fall back gracefully on DMG
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED {
	uint8_t tile_data[64];

	memset(&tile_data, 0, sizeof(tile_data));

	if (value & 0x02) sidebar_draw_char(tile_data, 1, 0x0C, 1);
	if (value & 0x04) sidebar_draw_char(tile_data, 2, 0x0C, 2);
	if (value & 0x08) sidebar_draw_char(tile_data, 3, 0x0C, 3);
	if (value & 0x10) sidebar_draw_char(tile_data, 4, 0x0C, 1);
	if (value & 0x20) sidebar_draw_char(tile_data, 5, 0x0C, 2);
	if (value & 0x40) sidebar_draw_char(tile_data, 6, 0x0C, 2);
	if (value & 0x80) sidebar_draw_char(tile_data, 7, 0x0C, 3);

	sidebar_copy_data(0x9000 | (x_shifted << 4), tile_data, 64);
}
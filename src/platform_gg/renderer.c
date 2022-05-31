#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>

#include "bank_switch.h"
#include "gamevars.h"
#include "renderer.h"
#include "../../res/font_default.h"

#define COL_LVL_0 0
#define COL_LVL_1 5
#define COL_LVL_2 10
#define COL_LVL_3 15

const uint16_t gg_palette[16] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_1, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3)
};

uint8_t renderer_scrolling;

void text_init(uint8_t mode) {
	// initialize GG palette
	set_palette(0, 1, gg_palette);

	uint16_t address = 0x3800 + ((3 * 32 + 6) << 1);
	uint16_t i = 0;
	for (uint8_t iy = 0; iy < 18; iy++) {
		for (uint8_t ix = 0; ix < 20; ix++, i++) {
			set_vram_byte((uint8_t*) (address++), i);
			set_vram_byte((uint8_t*) (address++), (i >> 8));
		}
		address += 12 << 1;
	}

	// TODO

	DISPLAY_ON;
}

void text_sync_hblank_safe(void) {
	// TODO
}

void text_undraw(uint8_t x, uint8_t y) {

}

void text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	uint16_t vram_address = ((y * 20) + x) * 32;
	uint16_t char_address = chr * 8;
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	for (uint8_t iy = 0; iy < 8; iy++, char_address++) {
		set_vram_byte(vram_address++, _font_default_bin[char_address]);
		set_vram_byte(vram_address++, _font_default_bin[char_address]);
		set_vram_byte(vram_address++, _font_default_bin[char_address]);
		set_vram_byte(vram_address++, _font_default_bin[char_address]);
	}

	ZOO_SWITCH_ROM(prev_bank);
}

void text_free_line(uint8_t y) {

}

void text_scroll(int8_t dx, int8_t dy) {
	// TODO
}

void text_update(void) {
	// TODO
}

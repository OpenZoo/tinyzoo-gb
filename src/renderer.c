#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"

renderer_t active_renderer;

uint8_t renderer_scrolling = 0;
uint8_t draw_offset_x = 0;
uint8_t draw_offset_y = 0;
uint8_t scx_shadow_reg = 0;
uint8_t scy_shadow_reg = 0;
uint8_t ly_bank_switch;

uint8_t sidebar_tile_data_ly_switch;
uint8_t sidebar_tile_data_len;
uint16_t sidebar_tile_data_address;
__at(0xC040) static uint8_t sidebar_tile_data[96];
volatile bool sidebar_tile_data_awaiting = false;

void sidebar_vbl_copy_data(void) {
	if (sidebar_tile_data_awaiting) {
		// TODO: this is a kludge...
		if (_cpu != CGB_TYPE && sidebar_tile_data_len > 48) {
			memcpy((uint8_t*) sidebar_tile_data_address, sidebar_tile_data, 48);
			sidebar_tile_data_address += 48;
			sidebar_tile_data_len -= 48;
			memmove(sidebar_tile_data, sidebar_tile_data + 48, sidebar_tile_data_len);
			return;
		}
		memcpy((uint8_t*) sidebar_tile_data_address, sidebar_tile_data, sidebar_tile_data_len);
		if (sidebar_tile_data_ly_switch != 0) {
			ly_bank_switch = sidebar_tile_data_ly_switch;
			sidebar_tile_data_ly_switch = 0;
		}
		sidebar_tile_data_awaiting = false;
	}
}

void text_init(const renderer_t *renderer) {
	LCDC_REG = 0x00;
	hblank_isr_jp = 0xC3;

	// not using sprites here
	_shadow_OAM_base = 0;

	wait_vbl_done();
	// clear bottom bar tiles
	memset((uint8_t*) 0x9000, 0, 20 * 16);

	ly_bank_switch = 135;

	uint8_t prev_bank = _current_bank;
	SWITCH_ROM_MBC5(3);

	memcpy(&active_renderer, renderer, sizeof(renderer_t));
	active_renderer.init();

	SWITCH_ROM_MBC5(prev_bank);
}

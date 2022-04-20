#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "game_transition.h"
#include "renderer.h"

renderer_t active_renderer;

uint8_t renderer_id = 255;
uint8_t renderer_mode = 0;
uint8_t renderer_scrolling = 0;
uint8_t draw_offset_x = 0;
uint8_t draw_offset_y = 0;
uint8_t lcdc_shadow_reg = 0b11010001;
uint8_t scx_shadow_reg = 0;
uint8_t scy_shadow_reg = 0;
uint8_t ly_bank_switch;

uint8_t sidebar_tile_data_ly_switch;
// uint8_t sidebar_tile_data_len;
// uint16_t sidebar_tile_data_address;
AT(0xC040) static uint8_t sidebar_tile_data[96];
volatile bool sidebar_tile_data_awaiting = false;

static inline void sidebar_vbl_copy_data(void) {
	if (sidebar_tile_data_awaiting) {
		// TODO: this is a kludge...
		/* if (_cpu != CGB_TYPE && sidebar_tile_data_len > 48) {
			memcpy((uint8_t*) sidebar_tile_data_address, sidebar_tile_data, 48);
			sidebar_tile_data_address += 48;
			sidebar_tile_data_len -= 48;
			memmove(sidebar_tile_data, sidebar_tile_data + 48, sidebar_tile_data_len);
			return;
		}
		vmemcpy((uint8_t*) sidebar_tile_data_address, sidebar_tile_data, sidebar_tile_data_len); */
		if (sidebar_tile_data_ly_switch != 0) {
			ly_bank_switch = sidebar_tile_data_ly_switch;
			sidebar_tile_data_ly_switch = 0;
		}
		sidebar_tile_data_awaiting = false;
	}
}

void global_vblank_isr(void) {
	sidebar_vbl_copy_data();
#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_step();
#endif
}

void text_init(uint8_t mode, const renderer_t *renderer) {
	IE_REG &= ~LCD_IFLAG;
	draw_offset_x = 0;
	draw_offset_y = 0;

	if (renderer != NULL) {
		hblank_isr_jp = 0xC3;
		// not using sprites here
		_shadow_OAM_base = 0;

		// clear bottom bar tiles
		vmemset((uint8_t*) 0x9000, 0, 20 * 16);

		memcpy(&active_renderer, renderer, sizeof(renderer_t));
	}

	ly_bank_switch = 135;

	uint8_t prev_bank = _current_bank;
	SWITCH_ROM_MBC5(3);

	active_renderer.init(mode);
	active_renderer.update();

	SWITCH_ROM_MBC5(prev_bank);
}

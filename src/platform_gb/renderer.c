#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>

#include "bank_switch.h"
#include "config.h"
#include "../game_transition.h"
#include "../input.h"
#include "../renderer.h"

renderer_t active_renderer;

uint8_t renderer_id = 255;
uint8_t renderer_mode = 0;
uint8_t renderer_scrolling = 0;
uint8_t draw_offset_x = 0;
uint8_t draw_offset_y = 0;
uint8_t lcdc_shadow_reg = LCDCF_ON;
uint8_t scx_shadow_reg = 0;
uint8_t scy_shadow_reg = 0;
uint8_t ly_bank_switch;
uint8_t ly_offset;
extern uint8_t vbl_ticks;

uint8_t sidebar_tile_data_ly_switch = 0;
AT(0xC040) static uint8_t sidebar_tile_data[96];

static inline void sidebar_vbl_copy_data(void) {
	if (sidebar_tile_data_ly_switch != 0) {
		ly_bank_switch = sidebar_tile_data_ly_switch;
		sidebar_tile_data_ly_switch = 0;
	}
}

void global_vblank_isr(void) {
	vbl_ticks++;
	input_update_vbl();
	sidebar_vbl_copy_data();
#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_step();
#endif
}

void text_init(uint8_t mode, const renderer_t *renderer) {
	if (mode > RENDER_MODE_TITLE) {
		draw_offset_x = 0;
		draw_offset_y = 0;
	}

	if (renderer != NULL) {
		hblank_isr_jp = 0xC3;
		// not using sprites here
		_shadow_OAM_base = 0;

		// clear bottom bar tiles
		vmemset((uint8_t*) 0x9000, 0, 20 * 16);

		memcpy(&active_renderer, renderer, sizeof(renderer_t));
	}

	if (mode == RENDER_MODE_TITLE) {
		ly_bank_switch = 143;
	} else {
#ifndef HACK_HIDE_STATUSBAR
		ly_bank_switch = 135;
#else
		ly_bank_switch = 143;
#endif
	}
	ly_offset = ly_bank_switch;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	active_renderer.init(mode);
	active_renderer.update();

	ZOO_SWITCH_ROM(prev_bank);
}

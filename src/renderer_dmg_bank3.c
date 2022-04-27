#pragma bank 3

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"
#include "font_manager.h"

#ifndef __POCKET__

void dmg_vblank_isr(void);

void dmg_text_init(uint8_t mode) {
	renderer_mode = mode;

	if (renderer_id != RENDER_ID_DMG) {
		font_8x8_install(0, 3);
	}

	wait_vbl_done();
	if (renderer_id != RENDER_ID_DMG) {
		dmg_vblank_isr();
		add_VBL(dmg_vblank_isr);

		renderer_id = RENDER_ID_DMG;
	}

	if (mode == RENDER_MODE_PLAYFIELD) {
		// set bottom bar
		uint8_t *bottom_bar_ptr = (uint8_t*) (0x9C00 + (17 << 5)); 
		for (uint8_t i = 0; i < 20; i++, bottom_bar_ptr++) {
			*bottom_bar_ptr = i;
		}

		STAT_REG = STATF_LYC;
		IE_REG |= LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BGON;
	} else {
		IE_REG &= ~LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BG9C00 | LCDCF_BGON;
	}
}

#endif

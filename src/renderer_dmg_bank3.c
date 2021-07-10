#pragma bank 3

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"
#include "font_manager.h"

void dmg_hblank_switch_window(void);
void dmg_vblank_isr(void);

void dmg_text_init(void) {
	font_8x8_install(0, 3);

	wait_vbl_done();
	dmg_vblank_isr();
	add_VBL(dmg_vblank_isr);

	// set bottom bar
	uint8_t *bottom_bar_ptr = 0x9C00 + (17 << 5); 
	for (uint8_t i = 0; i < 20; i++, bottom_bar_ptr++) {
		*bottom_bar_ptr = i;
	}

	hblank_isr_ip = (uint16_t) dmg_hblank_switch_window;

	BGP_REG = 0b11001100;
	STAT_REG = 0b01000000;
	IE_REG |= LCD_IFLAG;
}

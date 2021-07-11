#pragma bank 3

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "renderer.h"
#include "font_manager.h"

extern uint16_t cgb_message_palette[];
extern uint8_t cgb_update_row[];
extern const uint16_t cgb_palette[];

void gbc_vblank_isr(void);

static uint8_t cgb_sidebar_colors[20] = {
	0, 0, 0, 1, 0,
	0, 0, 0, 0, 3,
	0, 0, 0, 0, 0,
	0, 1, 1, 2, 3
};

void sidebar_set_message_color(uint8_t color) BANKED {
	cgb_message_palette[15] = cgb_palette[color];
}

void gbc_text_init(void) {
	uint8_t i;

	font_8x8_install(0, 1);

	VBK_REG = 1;

	font_8x8_install(2, 3);

	VBK_REG = 0;

	cgb_update_row[0] = 0xFF;
	cgb_update_row[1] = 0xFF;
	cgb_update_row[2] = 0xFF;
	cgb_update_row[3] = 0xFF;
 
	cgb_message_palette[0] = cgb_palette[1];
	cgb_message_palette[1] = cgb_palette[13];
	cgb_message_palette[2] = cgb_palette[14];
	cgb_message_palette[3] = cgb_palette[15];
	cgb_message_palette[4] = cgb_palette[1];
	cgb_message_palette[5] = cgb_palette[9];
	cgb_message_palette[6] = cgb_palette[10];
	cgb_message_palette[7] = cgb_palette[11];
	cgb_message_palette[8] = cgb_palette[1];
	cgb_message_palette[9] = cgb_palette[12];
	cgb_message_palette[10] = cgb_palette[13];
	cgb_message_palette[12] = cgb_palette[1];
	cgb_message_palette[13] = cgb_palette[6];
	cgb_message_palette[14] = cgb_palette[0];
	cgb_message_palette[15] = cgb_palette[10];

	// set bottom bar
	uint8_t *bottom_bar_ptr = (uint8_t*) 0x9C00 + (13 << 5); 
	VBK_REG = 1;
	memset(bottom_bar_ptr, 0b00001011, (32 * 4));
	bottom_bar_ptr += 128;
	for (i = 0; i < 20; i++, bottom_bar_ptr++) {
		VBK_REG = 1;
		*bottom_bar_ptr = cgb_sidebar_colors[i];
		VBK_REG = 0;
		*bottom_bar_ptr = i;
	}

	wait_vbl_done();
	gbc_vblank_isr();
	add_VBL(gbc_vblank_isr);

	STAT_REG = 0b01000000;
	IE_REG |= LCD_IFLAG;
}

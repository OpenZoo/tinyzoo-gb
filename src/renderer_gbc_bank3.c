#pragma bank 3

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "renderer.h"
#include "font_manager.h"

extern uint16_t cgb_message_palette[];
extern const uint16_t cgb_palette[];

void gbc_vblank_isr(void);

static uint8_t cgb_sidebar_colors[20] = {
	0, 0, 0, 1, 0,
	0, 0, 0, 0, 3,
	0, 0, 0, 0, 0,
	0, 1, 1, 2, 0
};

void sidebar_set_message_color(uint8_t color) BANKED {
	cgb_message_palette[15] = cgb_palette[color];
}

static void gbc_text_init_wram(void) {
/* __asm
	; clear banks 2, 3, 4
	; TODO: inefficient
	ld bc, #(_SVBK_REG)
	ld de, #0xFF00
	ld hl, #0xD000
	ld a, #2
WramClearLoop:
	ld (bc), a
	ld (hl), e
	inc a
	ld (bc), a
	ld (hl), d
	inc a
	ld (bc), a
	ld (hl), e
	ld a, #2
	inc hl
	bit 5, h
	jr z, WramClearLoop
	; finish
	xor a, a
	ld (bc), a
__endasm; */
__asm
	; clear bank 2
	ld a, #2
	ld (_SVBK_REG), a
	ld hl, #0xD000
	xor a, a
WramClearLoop:
	ld (hl+), a
	bit 5, h
	jr z, WramClearLoop
	; finish
	ld (_SVBK_REG), a
__endasm;
}

void gbc_text_init(uint8_t mode) {
	uint8_t i;

	if (renderer_id != RENDER_ID_GBC) {
		font_8x8_install(0, 1);

		VBK_REG = 1;

		font_8x8_install(2, 3);

		VBK_REG = 0;

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

		gbc_text_init_wram();
	}

	renderer_mode = mode;
	uint8_t *bottom_bar_ptr = (uint8_t*) 0x9C00 + (13 << 5);

	if (mode == RENDER_MODE_PLAYFIELD) {
		VBK_REG = 1;
		vmemset(bottom_bar_ptr, 0b00001011, (32 * 4));
		bottom_bar_ptr += 128;
		vmemcpy(bottom_bar_ptr, cgb_sidebar_colors, 20);
	}

	if (mode == RENDER_MODE_PLAYFIELD || renderer_id != RENDER_ID_GBC) {
		wait_vbl_done();
	}

	if (mode == RENDER_MODE_PLAYFIELD) {
		// set bottom bar
		VBK_REG = 0;
		for (i = 0; i < 20; i++, bottom_bar_ptr++) {
			*bottom_bar_ptr = i;
		}
	}

	if (renderer_id != RENDER_ID_GBC) {
		gbc_vblank_isr();
		add_VBL(gbc_vblank_isr);

		renderer_id = RENDER_ID_GBC;
	}

	if (mode == RENDER_MODE_PLAYFIELD) {
		STAT_REG = 0b01000000;
		IE_REG |= LCD_IFLAG;
		lcdc_shadow_reg = 0b11010001;
	} else {
		IE_REG &= ~LCD_IFLAG;
		lcdc_shadow_reg = 0b11011001;
	}
}

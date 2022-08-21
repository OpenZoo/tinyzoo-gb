#pragma bank 3

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>
#include "config.h"
#include "font_manager.h"
#include "../renderer.h"

#define COL_LVL_0 0
#define COL_LVL_1 10
#define COL_LVL_2 21
#define COL_LVL_3 31

const uint16_t cgb_empty_palette[10] = {
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0
};

const uint16_t cgb_txtwind_palette[10] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1)
};

const uint16_t cgb_menu_palette[4] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1)
};

extern uint16_t cgb_message_palette[];
extern const uint16_t cgb_palette[];
extern uint16_t *cgb_static_palette;
extern uint8_t cgb_static_palette_offset;
extern uint8_t cgb_static_palette_bank;

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

void gbc_init_set_palette(void) BANKED {
	if (renderer_mode == RENDER_MODE_TXTWIND) {
		cgb_static_palette = cgb_txtwind_palette;
		cgb_static_palette_offset = 0;
		cgb_static_palette_bank = 3;
	} else if (renderer_mode == RENDER_MODE_MENU) {
		cgb_static_palette = cgb_menu_palette;
		cgb_static_palette_offset = 0;
		cgb_static_palette_bank = 3;
	}
}

void gbc_init_unset_palette(void) BANKED {
	cgb_static_palette = cgb_empty_palette;
	cgb_static_palette_bank = 3;
}

static void gbc_text_init_wram(void) {
__asm
	; clear bank 2
	ld a, #2
	ldh (_SVBK_REG + 0), a
	ld hl, #0xD000
	xor a, a
WramClearLoop:
	ld (hl+), a
	ld (hl+), a
	ld (hl+), a
	ld (hl+), a
	bit 5, h
	jr z, WramClearLoop
	; finish
	ldh (_SVBK_REG + 0), a
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
#ifdef HACK_HIDE_STATUSBAR
	uint8_t *bottom_bar_ptr = (uint8_t*) 0x9C00 + (14 << 5);
#else
	uint8_t *bottom_bar_ptr = (uint8_t*) 0x9C00 + (13 << 5);
#endif

	if (mode == RENDER_MODE_PLAYFIELD) {
		VBK_REG = 1;
		vmemset(bottom_bar_ptr, 0b00001011, (32 * 4));
#ifdef HACK_HIDE_STATUSBAR
		VBK_REG = 0;
#else
		bottom_bar_ptr += 128;
		vmemcpy(bottom_bar_ptr, cgb_sidebar_colors, 20);

		// set bottom bar
		VBK_REG = 0;
		for (i = 0; i < 20; i++, bottom_bar_ptr++) {
			set_vram_byte(bottom_bar_ptr, i);
		}
#endif
	}

	if (mode <= RENDER_MODE_TITLE) {
		cgb_static_palette = NULL;
	} else {
		cgb_static_palette = cgb_empty_palette;
		cgb_static_palette_bank = 3;
		if (renderer_id == RENDER_ID_GBC) {
			wait_vbl_done();
			gbc_vblank_isr();
		}
	}

	if (renderer_id != RENDER_ID_GBC) {
		wait_vbl_done();

		gbc_vblank_isr();
		add_VBL(gbc_vblank_isr);
		add_VBL(nowait_int_handler);

		renderer_id = RENDER_ID_GBC;
	}

	if (mode <= RENDER_MODE_TITLE) {
		STAT_REG = STATF_LYC;
		IE_REG |= LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BGON;
	} else {
		IE_REG &= ~LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BG9C00 | LCDCF_BGON;
	}
}

#pragma bank 3

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

	if (mode <= RENDER_MODE_TITLE) {
		uint8_t *bottom_bar_ptr;

#ifdef HACK_HIDE_STATUSBAR
		if (1) {
#else
		if (mode == RENDER_MODE_TITLE) {
#endif
			bottom_bar_ptr = (uint8_t*) 0x9C00 + (14 << 5);
		} else {
			bottom_bar_ptr = (uint8_t*) 0x9C00 + (13 << 5);
		}

		IE_REG &= ~TIM_IFLAG;

		VBK_REG = 1;
		vmemset(bottom_bar_ptr, 0b00001011, (32 * 4));

#ifndef HACK_HIDE_STATUSBAR
		if (mode == RENDER_MODE_PLAYFIELD) {
			bottom_bar_ptr += 128;
			vmemcpy(bottom_bar_ptr, cgb_sidebar_colors, 20);

			// set bottom bar
			VBK_REG = 0;
			for (i = 0; i < 20; i++, bottom_bar_ptr++) {
				set_vram_byte(bottom_bar_ptr, i);
			}
		}
#endif
		VBK_REG = 0;
		IE_REG |= TIM_IFLAG;
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

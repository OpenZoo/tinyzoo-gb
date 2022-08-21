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
#include "font_manager.h"
#include "../renderer.h"

#ifndef __POCKET__

void dmg_vblank_isr(void);

void dmg_text_init(uint8_t mode) {
	renderer_mode = mode;

	if (renderer_id != RENDER_ID_DMG) {
		font_8x8_install(0, 3);
	}

	if (renderer_id != RENDER_ID_DMG) {
		wait_vbl_done();
		dmg_vblank_isr();
		add_VBL(dmg_vblank_isr);
		add_VBL(nowait_int_handler);

		renderer_id = RENDER_ID_DMG;
	}

	if (mode <= RENDER_MODE_TITLE) {
		// set bottom bar
		uint8_t *bottom_bar_ptr = (uint8_t*) (0x9C00 + (17 << 5));
		for (uint8_t i = 0; i < 20; i++, bottom_bar_ptr++) {
			set_vram_byte(bottom_bar_ptr, i);
		}

		STAT_REG = STATF_LYC;
		IE_REG |= LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BGON;
	} else if (mode == RENDER_MODE_NONE) {
		IE_REG &= ~LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON;
	} else {
		IE_REG &= ~LCD_IFLAG;
		lcdc_shadow_reg = LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BG9C00 | LCDCF_BGON;
	}
}

#endif

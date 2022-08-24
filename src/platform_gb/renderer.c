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

#ifdef HACK_HIDE_STATUSBAR
	if (1) {
#else
	if (mode == RENDER_MODE_TITLE) {
#endif
		ly_bank_switch = 143;
	} else {
		ly_bank_switch = 135;
	}
	ly_offset = ly_bank_switch;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	active_renderer.init(mode);
	active_renderer.update();

	ZOO_SWITCH_ROM(prev_bank);
}

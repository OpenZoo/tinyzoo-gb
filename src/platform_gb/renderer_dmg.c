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
#include "font_manager.h"
#include "../renderer.h"

#ifndef __POCKET__

extern uint8_t ly_bank_switch;
extern uint8_t ly_offset;

void dmg_hblank_switch_window_pre(void) NAKED {
__asm
.hblank_switch_window_pre_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
#ifdef __POCKET__
	bit 6, a				; 1 cycles
#else
	bit 1, a				; 1 cycles
#endif
	jr nz, .hblank_switch_window_pre_sync		; 1.5 cycles

	ld a, #0xD9 ; 8
	ldh (_LCDC_REG + 0), a ; 12
	xor a, a ; 4
	ldh (_SCX_REG + 0), a ; 12
	ldh (_SCY_REG + 0), a ; 12
	ld a, #27 ; 8
	ldh (_BGP_REG + 0), a ; 12

	ld a, (_ly_offset)
	ldh (_LYC_REG + 0), a
	
	ld a, #<(_dmg_hblank_switch_window)
	ld (_hblank_isr_ip), a
	ld a, #>(_dmg_hblank_switch_window)
	ld (_hblank_isr_ip+1), a

	pop af
	reti
__endasm;
}

void dmg_hblank_switch_window(void) NAKED {
__asm
.hblank_switch_window_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
#ifdef __POCKET__
	bit 6, a				; 1 cycles
#else
	bit 1, a				; 1 cycles
#endif
	jr nz, .hblank_switch_window_sync		; 1.5 cycles

	ld a, #0xC9 ; 8
	ldh (_LCDC_REG + 0), a ; 12
	xor a, a ; 4
	ldh (_SCX_REG + 0), a ; 12
	ldh (_SCY_REG + 0), a ; 12
	ld a, #27 ; 8
	ldh (_BGP_REG + 0), a ; 12

	pop af
	reti
__endasm;
}

void global_vblank_isr(void);

void dmg_vblank_isr(void) {
	uint8_t prev_bank = _current_bank;
	LCDC_REG = lcdc_shadow_reg;
	BGP_REG = 0b11100100;
	if (renderer_mode <= RENDER_MODE_TITLE) {
		LYC_REG = ly_bank_switch;
	} else {
		LYC_REG = 255;
	}
	if (ly_bank_switch < ly_offset) {
		hblank_isr_ip = (uint16_t) dmg_hblank_switch_window_pre;
	} else {
		hblank_isr_ip = (uint16_t) dmg_hblank_switch_window;
	}

	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;
	global_vblank_isr();
	ZOO_SWITCH_ROM(prev_bank);
}

void dmg_text_init(uint8_t mode); // bank 3

static void dmg_text_undraw(uint8_t x, uint8_t y) {

}

static void dmg_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
/*	x = (draw_offset_x + x) & 0x1F;
	y = (draw_offset_y + y) & 0x1F;
	
	volatile uint8_t *tptr = (uint8_t*) (0x9800 | (y << 5) | x);
	while (STAT_REG & 0x02);
	*tptr = chr; */
__asm
	; a = x, e = y, stack = chr, col
	ld	c, a
	ld      a, (#_draw_offset_x)
	add     a, c
	and     a, #0x1f
	ld		c, a	; c = (draw_offset_x + x) & 0x1F
	ld      a, (#_draw_offset_y)
	add     a, e
	add		a, a
	add		a, a
	add		a, a ; a <<= 3
	ld		l, a
	ld      h, #0x00 ; hl = (draw_offset_y + y) & 0x1F
	ld		b, h	; bc = (draw_offset_x + x) & 0x1F
	add     hl, hl
	add     hl, hl	; .. <<= 2 (total 5)
	add		hl, bc	; .. + x
	ld      c, l
	ld      a, h
	or      a, #0x98
	ld      b, a	; bc = VRAM address
	ldhl    sp,     #2
	di
.DmgTextDrawSync:
	ldh     a, (_STAT_REG + 0)
#ifdef __POCKET__
	bit 6, a
#else
	bit 1, a
#endif
	jr      nz, .DmgTextDrawSync
	ld      a, (hl)	; a = chr
	ld      (bc), a
	ei
__endasm;
}

static void dmg_text_sync_hblank_safe(void) {

}

static void dmg_text_free_line(uint8_t y) {

}

static void dmg_text_scroll(int8_t dx, int8_t dy) {
	draw_offset_x += dx;
	draw_offset_y += dy;
}

static void dmg_text_update(void) {
	scx_shadow_reg = draw_offset_x << 3;
	scy_shadow_reg = draw_offset_y << 3;
}

const renderer_t renderer_dmg = {
	dmg_text_init,
	dmg_text_sync_hblank_safe,
	dmg_text_undraw,
	dmg_text_draw,
	dmg_text_free_line,
	dmg_text_scroll,
	dmg_text_update
};

#endif

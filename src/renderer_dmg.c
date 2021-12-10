#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"
#include "font_manager.h"

extern uint8_t ly_bank_switch;

void dmg_hblank_switch_window_pre(void) {
__asm
.hblank_switch_window_pre_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
	bit 1, a				; 1 cycles
	jp nz, .hblank_switch_window_pre_sync		; 1.5 cycles

	ld a, #0xD9 ; 8
	ldh (_LCDC_REG + 0), a ; 12
	xor a, a ; 4
	ldh (_SCX_REG + 0), a ; 12
	ldh (_SCY_REG + 0), a ; 12
	ld a, #27 ; 8
	ldh (_BGP_REG + 0), a ; 12

	ld a, #135
	ldh (_LYC_REG + 0), a
	
	ld a, #<(_dmg_hblank_switch_window)
	ld (_hblank_isr_ip), a
	ld a, #>(_dmg_hblank_switch_window)
	ld (_hblank_isr_ip+1), a

	pop af
	reti
__endasm;
}

void dmg_hblank_switch_window(void) {
__asm
.hblank_switch_window_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
	bit 1, a				; 1 cycles
	jp nz, .hblank_switch_window_sync		; 1.5 cycles

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

void sidebar_vbl_copy_data(void);

void dmg_vblank_isr(void) {
	LCDC_REG = 0b11010001;
	BGP_REG = 0b11100100;
	LYC_REG = ly_bank_switch;
	if (ly_bank_switch < 135) {
		hblank_isr_ip = (uint16_t) dmg_hblank_switch_window_pre;
	} else {
		hblank_isr_ip = (uint16_t) dmg_hblank_switch_window;
	}

	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;
	sidebar_vbl_copy_data();
}

void dmg_text_init(); // bank 3

static void dmg_text_undraw(uint8_t x, uint8_t y) {

}

static void dmg_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
/*	x = (draw_offset_x + x) & 0x1F;
	y = (draw_offset_y + y) & 0x1F;
	
	volatile uint8_t *tptr = (uint8_t*) (0x9800 | (y << 5) | x);
	while (STAT_REG & 0x02);
	*tptr = chr; */
__asm
	ld      a, (#_draw_offset_x)
	ldhl    sp,     #2
	add     a, (hl)
	and     a, #0x1f
	ld		c, a	; c = (draw_offset_x + x) & 0x1F
	ld      a, (#_draw_offset_y)
	inc		hl
	add     a, (hl)
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
	ldhl    sp,     #4
.DmgTextDrawSync:
	ldh     a, (_STAT_REG + 0)
	bit     1, a
	jr      nz, .DmgTextDrawSync
	ld      a, (hl)	; a = chr
	ld      (bc), a
__endasm;
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
	dmg_text_undraw,
	dmg_text_draw,
	dmg_text_free_line,
	dmg_text_scroll,
	dmg_text_update
};

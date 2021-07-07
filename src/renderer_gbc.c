#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "renderer.h"
#include "../res/font_default.h"

#define COL_LVL_0 0
#define COL_LVL_1 10
#define COL_LVL_2 20
#define COL_LVL_3 30

static const uint16_t cgb_palette[16] = {
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_0),
	RGB(COL_LVL_0, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_0, COL_LVL_2),
	RGB(COL_LVL_2, COL_LVL_1, COL_LVL_0),
	RGB(COL_LVL_2, COL_LVL_2, COL_LVL_2),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_1, COL_LVL_3, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_1, COL_LVL_3),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_1),
	RGB(COL_LVL_3, COL_LVL_3, COL_LVL_3)
};

static uint8_t draw_offset_x_end = VIEWPORT_WIDTH;
static uint8_t draw_offset_y_end = VIEWPORT_HEIGHT;

#define cgb_back_chr ((uint8_t*) 0xA000)
#define cgb_back_col ((uint8_t*) 0xA400)

static uint8_t cgb_color_buffer[16];
static uint8_t cgb_update_row[4];
static bool full_redraw;

static uint16_t hblank_isr_sp;
static uint16_t hblank_isr_pal_pos;

static void vblank_update_palette(void) {
__asm
	push bc
	push de

	; backup sp
	ld (_hblank_isr_sp), sp

	; hblank_isr_pal_pos += 0x40
	ld hl, #(_hblank_isr_pal_pos)
	ld a, (hl+)
	ld h, (hl)
	ld l, a

	push hl

	ld bc, #0x0040
	add hl, bc
	ld a, h
	and a, #0xD7
	ld c, l
	ld hl, #(_hblank_isr_pal_pos)
	ld (hl), c
	inc hl
	ld (hl), a

	pop hl

	; switch work banks
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; write 9 color pairs (18 colors)
.rept 18
	pop bc
	ld (hl), c
	ld (hl), b
.endm

	; restore work banks
	xor a, a
	ldh (_SVBK_REG + 0), a

	; restore SP
	ld hl, #(_hblank_isr_sp)
	ld c, (hl)
	inc hl
	ld h, (hl)
	ld l, c
	ld sp, hl

	pop de
	pop bc
__endasm;
}

static void hblank_update_palette(void) {
__asm
	push bc
	push de

	; backup sp
	ld (_hblank_isr_sp), sp

	; hl = pal_pos; pal_pos += 0x40
	ld hl, #(_hblank_isr_pal_pos)
	ld a, (hl+)
	ld h, (hl)
	ld l, a

	push hl

	ld bc, #0x0040
	add hl, bc
	ld a, h
	and a, #0xD7
	ld c, l
	ld hl, #(_hblank_isr_pal_pos)
	ld (hl), c
	inc hl
	ld (hl), a

	pop hl

	; increment LY
	ldh a, (_LYC_REG + 0)
	add a, #0x08
	ldh (_LYC_REG + 0), a

	; switch work banks
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; preload values (timing is tight)
	pop bc
	pop de

	; wait for STAT to be correct
.hblank_update_palette_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
	bit 1, a				; 1 cycles
	jp nz, .hblank_update_palette_sync		; 1.5 cycles

	; write 9 color pairs (18 colors)
	; budget: 67-71 cycles
	ld (hl), c	; 1 cycle
	ld (hl), b	; 1 cycle
	ld (hl), e	; 1 cycle
	ld (hl), d	; 1 cycle
.rept 16		; 3.5 x 16 = 56 cycles
	pop bc
	ld (hl), c
	ld (hl), b
.endm

	; restore work banks
	xor a, a
	ldh (_SVBK_REG + 0), a

	; restore SP
	ld hl, #(_hblank_isr_sp)
	ld c, (hl)
	inc hl
	ld h, (hl)
	ld l, c
	ld sp, hl

	pop de
	pop bc
	pop hl
	pop af
	reti
__endasm;
}

static void vblank_isr(void) {
	uint8_t local_doy = scy_shadow_reg >> 3;

	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;

	hblank_isr_ip = (uint16_t) hblank_update_palette;
	hblank_isr_pal_pos = 0xD000 | (local_doy << 6);
	LYC_REG = 7;

	vblank_update_palette();
}

static void text_cgb_push_char_only(uint16_t pos, uint8_t v1) {
__asm
	push bc
	
	ldhl sp, #6
	ld c, (hl)
	dec hl
	ld a, (hl-)
	ld l, (hl)
	ld h, a

	xor a, a
	ldh (_VBK_REG + 0), a
	
SyncLoopA:
	di
	ldh a, (_STAT_REG + 0)
	and a, #0x02
	jp z, SyncLoopAEnd
	ei
	jp SyncLoopA

SyncLoopAEnd:
	ld (hl), c

	ei

	pop bc
__endasm;
}

static void gbc_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	col &= 0x7F;
	
	x = (draw_offset_x + x) & 0x1F;
	y = (draw_offset_y + y) & 0x1F;

	ENABLE_RAM_MBC5;
	SWITCH_RAM_MBC5(0);

	uint16_t offset = (y << 5) | x;
	cgb_back_chr[offset] = chr;

	if ((col == cgb_back_col[offset]) && !full_redraw) {
		text_cgb_push_char_only(0x9800 | offset, chr);
	} else {
		uint8_t y_shift = (1 << (y & 7));
		cgb_back_col[offset] = col;
		cgb_update_row[y >> 3] |= y_shift;
	}

	DISABLE_RAM_MBC5;
}

static volatile uint16_t asm_pass_palptr;
static volatile uint16_t asm_tmp_pal1;
static volatile uint16_t asm_tmp_pal2;

static void text_cgb_push_color(void) {
__asm
	ld hl, #(_asm_tmp_pal1)
	ld b, (hl)
	inc hl
	ld c, (hl)
	inc hl
	ld d, (hl)
	inc hl
	ld e, (hl)

	push bc

	ld hl, #(_asm_pass_palptr)
	ld c, (hl)
	inc hl
	ld b, (hl)
	ld l, c
	ld h, b

	pop bc

	ld a, #0x02

	di
	ldh (_SVBK_REG + 0), a

	ld (hl), b
	inc hl
	ld (hl), c
	inc hl
	ld (hl), d
	inc hl
	ld (hl), e

	xor a, a
	ldh (_SVBK_REG + 0), a
	ei
__endasm;
}

static void text_cgb_push_char(uint16_t pos, uint8_t v1, uint8_t v2) {
__asm
	ldhl sp, #5
	ld c, (hl)
	dec hl
	ld b, (hl)
	dec hl
	ld a, (hl-)
	ld l, (hl)
	ld h, a

	ld a, #0x01
	ldh (_VBK_REG + 0), a
	xor a, a
	
SyncLoop:
	di
	ldh a, (_STAT_REG + 0)
	and a, #0x02
	jp z, SyncLoopEnd
	ei
	jp SyncLoop

SyncLoopEnd:
	ld (hl), c
	ldh (_VBK_REG + 0), a
	ld (hl), b

	ei
__endasm;
}

static void text_cgb_update_row(uint8_t y) {
	uint8_t colors_used = 0;

	uint8_t i;
	uint8_t x = draw_offset_x;

	uint16_t offset = (y << 5);
	uint16_t vram_addr = 0x9800 | offset;

	// build row buffer
	uint8_t *back_chr_offset = cgb_back_chr + offset;
	uint8_t *back_col_offset = cgb_back_col + offset;

	asm_pass_palptr = 0xD000 | (y << 6);
 
	for (i = draw_offset_x; i != draw_offset_x_end; i = (i + 1) & 0x1F) {
		uint8_t col = back_col_offset[i];
		uint8_t j = 0;

		for (; j < colors_used; j++) {
			if (cgb_color_buffer[j] == col) {
				break;
			}
		}

		if (j == colors_used) {
			if (j < 9) {
				cgb_color_buffer[j] = col;
				asm_tmp_pal1 = cgb_palette[col >> 4];
				asm_tmp_pal2 = cgb_palette[col & 0x0F];
				text_cgb_push_color();
				asm_pass_palptr += 4;
				colors_used++;
			} else {
				// TODO
				j = 0;
			}
		}
		
		uint8_t dcol = (j >> 1) | ((j & 0x01) << 3);
		text_cgb_push_char(vram_addr | i, back_chr_offset[i], dcol);
	}
}

static void gbc_text_update(void) {
	bool ram_enabled = false;

	for (uint8_t r1 = 0; r1 < 4; r1++) {
		uint8_t v = cgb_update_row[r1];
		if (v != 0) {
			if (!ram_enabled) {
				ram_enabled = true;
				ENABLE_RAM_MBC5;
				SWITCH_RAM_MBC5(0);
			}

			uint8_t row = r1 << 3;
			if (v&1) text_cgb_update_row(row);
			if (v&2) text_cgb_update_row(row+1);
			if (v&4) text_cgb_update_row(row+2);
			if (v&8) text_cgb_update_row(row+3);
			if (v&16) text_cgb_update_row(row+4);
			if (v&32) text_cgb_update_row(row+5);
			if (v&64) text_cgb_update_row(row+6);
			if (v&128) text_cgb_update_row(row+7);
			cgb_update_row[r1] = 0;
		} 
	}

	if (ram_enabled) {
		DISABLE_RAM_MBC5;
	}

	scx_shadow_reg = draw_offset_x << 3;
	scy_shadow_reg = draw_offset_y << 3;
	full_redraw = false;
}

static void gbc_text_init(void) {
	LCDC_REG = 0x00;

	volatile uint8_t *vptr = (uint8_t*) 0x8000;
	const uint8_t *fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		while (STAT_REG & 0x02);
		*(vptr++) = *fptr;
		*(vptr++) = 0;
	}

	VBK_REG = 1;

	vptr = (uint8_t*) 0x8000;
	fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		while (STAT_REG & 0x02);
		*(vptr++) = *fptr;
		*(vptr++) = 0xFF;
	}

	VBK_REG = 0;

	cgb_update_row[0] = 0xFF;
	cgb_update_row[1] = 0xFF;
	cgb_update_row[2] = 0xFF;
	cgb_update_row[3] = 0xFF;

	vblank_isr();
	add_VBL(vblank_isr);

	STAT_REG = 0b01000000;
	IE_REG |= LCD_IFLAG;
}

static void gbc_text_mark_redraw(void) {
	full_redraw = true;
}

static void gbc_text_scroll(int8_t dx, int8_t dy) {
	draw_offset_x = (draw_offset_x + dx) & 0x1F;
	draw_offset_y = (draw_offset_y + dy) & 0x1F;

	draw_offset_x_end = (draw_offset_x + VIEWPORT_WIDTH) & 0x1F;
	draw_offset_y_end = (draw_offset_y + VIEWPORT_HEIGHT) & 0x1F;

	if (dx != 0) {
DoFullRedraw:
		full_redraw = true;
		for (uint8_t y = draw_offset_y; y != draw_offset_y_end; y = (y + 1) & 0x1F) {
			cgb_update_row[y >> 3] |= 1 << (y & 7);
		}
	} else {
		if (dy == -1) {
			cgb_update_row[draw_offset_y >> 3] |= 1 << (draw_offset_y & 7);
		} else if (dy == 1) {
			uint8_t y = (draw_offset_y_end - 1) & 0x1F;
			cgb_update_row[y >> 3] |= 1 << (y & 7);
		} else {
			goto DoFullRedraw;
		}
	}
}

const renderer_t renderer_gbc = {
	gbc_text_init,
	gbc_text_draw,
	gbc_text_mark_redraw,
	gbc_text_scroll,
	gbc_text_update
};
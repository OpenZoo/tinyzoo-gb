#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>
#include "font_manager.h"
#include "himem.h"
#include "../renderer.h"

#define COL_LVL_0 0
#define COL_LVL_1 10
#define COL_LVL_2 21
#define COL_LVL_3 31

#define CGB_PALETTE_IS_PAGE_ALIGNED

__at(0x3FDC)
const uint16_t cgb_palette[16] = {
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

uint16_t cgb_message_palette[16];

static uint16_t hblank_isr_sp;
static uint16_t hblank_isr_pal_pos;
extern uint8_t ly_bank_switch;
static uint8_t new_lcdc_val;

static void load_palette(const uint16_t *pal) {
__asm
	; backup sp
	ld (_hblank_isr_sp), sp

	ld h, d
	ld l, e

	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	di

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; write 10 color pairs (20 colors)
.rept 20
	pop bc
	ld (hl), c
	ld (hl), b
.endm

	; restore SP
	ld hl, #(_hblank_isr_sp)
	ld a, (hl+)
	ld h, (hl)
	ld l, a
	ld sp, hl

	ei
__endasm;
}

static void vblank_update_palette(void) PRESERVES_REGS(d, e) {
__asm
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
	ld hl, #(_hblank_isr_pal_pos + 1)
	ld (hl-), a
	ld (hl), c

	pop hl

	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	di

	; switch work banks
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; write 10 color pairs (20 colors)
.rept 20
	pop bc
	ld (hl), c
	ld (hl), b
.endm

	; restore work banks
	xor a, a
	ldh (_SVBK_REG + 0), a

	; restore SP
	ld hl, #(_hblank_isr_sp)
	ld a, (hl+)
	ld h, (hl)
	ld l, a
	ld sp, hl

	ei
__endasm;
}

static void gbc_hblank_switch_window(void) NAKED {
__asm
.hblank_switch_window_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
#ifdef __POCKET__
	bit 6, a
#else
	bit 1, a
#endif
	jr nz, .hblank_switch_window_sync		; 1.5 cycles

#ifdef __POCKET__
	ld a, #0x93 ; 8
#else
	ld a, #0xC9 ; 8
#endif
	ldh (_LCDC_REG + 0), a ; 12

	pop af
	reti
__endasm;
}

static void hblank_update_palette(void) NAKED {
__asm
	push hl
	push bc
	push de

	; backup sp
	ld (_hblank_isr_sp), sp

	; do the thing you are not supposed to do, part 1
	; - set STAT to respond to the beginning of HBlank
#ifdef __POCKET__
	ld a, #0x10
#else
	ld a, #0x08
#endif
	ldh (_STAT_REG + 0), a
	; - set IE to only respond to LCD STAT
	ld a, #0x02
	ldh (_IE_REG + 0), a
	; - clear LCD STAT IF
	ldh a, (_IF_REG + 0)
	and a, #0xFD
	ldh (_IF_REG + 0), a

	; hl = pal_pos
	ld hl, #(_hblank_isr_pal_pos)
	ld a, (hl+)
	ld h, (hl)
	ld l, a

	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	; increment LY
	ldh a, (_ly_bank_switch_mirror)
	ld b, a
	ldh a, (_LYC_REG + 0)
	cp a, b
	jr nc, .hblank_update_palette_window
	add a, #0x08
	ldh (_LYC_REG + 0), a

	; switch work banks
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; preload values (timing is tight)
	pop bc
	pop de

	; do the thing you are not supposed to do, part 2
	; - halt
	halt
	; - halt bug workaround
	nop

	; write 10 color pairs (20 colors)
	; budget: 67-71 cycles
	ld (hl), c	; 1 cycle
	ld (hl), b	; 1 cycle
	ld (hl), e	; 1 cycle
	ld (hl), d	; 1 cycle
.rept 18		; 3.5 x 16 = 56 cycles
	pop bc
	ld (hl), c
	ld (hl), b
.endm

	; restore work banks
	xor a, a
	ldh (_SVBK_REG + 0), a

	; hl = pal_pos; pal_pos += 0x40
	ld hl, #(_hblank_isr_pal_pos)
	ld a, (hl+)
	ld h, (hl)
	ld l, a

	ld bc, #0x0040
	add hl, bc
	ld a, h
	and a, #0xD7
	ld c, l
	ld hl, #(_hblank_isr_pal_pos + 1)
	ld (hl-), a
	ld (hl), c

	pop hl

.hblank_update_palette_restore:
	; restore SP
	ld hl, #(_hblank_isr_sp)
	ld a, (hl+)
	ld h, (hl)
	ld l, a
	ld sp, hl

	; undo the thing you are not supposed to do
	; - set STAT to respond to the beginning of HBlank
#ifdef __POCKET__
	ld a, #0x02
#else
	ld a, #0x40
#endif
	ldh (_STAT_REG + 0), a
	; - set IE to respond to all three
	ld a, #0x07
	ldh (_IE_REG + 0), a
	; - clear LCD STAT IF
	ldh a, (_IF_REG + 0)
	and a, #0xFD
	ldh (_IF_REG + 0), a

	pop de
	pop bc
	pop hl
	pop af
	reti

.hblank_update_palette_window:
	ld hl, #(_cgb_message_palette)

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; preload values (timing is tight)
	pop bc
	pop de

	; do the thing you are not supposed to do, part 2
	; - halt
	halt
	; - halt bug workaround
	nop

	; budget: 67-71 cycles
	ld (hl), c	; 1 cycle
	ld (hl), b	; 1 cycle
	ld (hl), e	; 1 cycle
	ld (hl), d	; 1 cycle
.rept 16
	pop bc
	ld (hl), c
	ld (hl), b
.endm
	ld a, (_new_lcdc_val) ; 8
	ldh (_LCDC_REG + 0), a ; 12
	xor a, a ; 4
	ldh (_SCX_REG + 0), a ; 12
	ldh (_SCY_REG + 0), a ; 12

	ld a, #135
	ldh (_LYC_REG + 0), a
	ld a, #<(_gbc_hblank_switch_window)
	ld (_hblank_isr_ip), a
	ld a, #>(_gbc_hblank_switch_window)
	ld (_hblank_isr_ip+1), a

	jr .hblank_update_palette_restore

__endasm;
}

void global_vblank_isr(void);

void gbc_vblank_isr(void) {
	uint8_t local_doy = scy_shadow_reg >> 3;

	LCDC_REG = lcdc_shadow_reg;
	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;

	if (renderer_mode == RENDER_MODE_PLAYFIELD) {
		hblank_isr_ip = (uint16_t) hblank_update_palette;
		hblank_isr_pal_pos = 0xD000 | ((uint16_t)local_doy << 6);
		LYC_REG = 7;
		ly_bank_switch_mirror = ly_bank_switch;
		new_lcdc_val = (ly_bank_switch < 135)
			? (LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG8000 | LCDCF_BG9C00 | LCDCF_BGON)
			: (LCDCF_ON | LCDCF_WIN9C00 | LCDCF_BG9C00 | LCDCF_BGON);
		vblank_update_palette();
	} else if (renderer_mode == RENDER_MODE_TXTWIND) {
		load_palette(cgb_txtwind_palette);
	}

	global_vblank_isr();
}

static void gbc_sync_di(void) NAKED PRESERVES_REGS(b, c, d, e, h, l) {
__asm;
	ldh a, (_LY_REG + 0)
	cp a, #137
	jr nc, GbcSyncDiDone
//	and a, #0x07
//	cp a, #0x06
//	jr c, GbcSyncDiDone
GbcSyncDiLoop:
	ldh a, (_LY_REG + 0)
	and a, #0x07
	cp a, #0x06
	jr nc, GbcSyncDiLoop
GbcSyncDiDone:
	di
	ret
__endasm;
}

// Lower tolerance due to potential C overhead
static void gbc_text_sync_hblank_safe(void) NAKED PRESERVES_REGS(b, c, d, e, h, l) {
__asm;
GbcHSSyncDiLoop:
    ; LY in range?
	ldh a, (_LY_REG + 0)
	and a, #0x07
	cp a, #0x04
	jr nc, GbcHSSyncDiLoop
	ret
__endasm;
}

void safe_vmemcpy(uint8_t *dest, const uint8_t *src, uint8_t blocks) {
__asm;
	// dest = de
	// src = bc
	// blocks = stack
	ldhl sp, #2
	ld a, (hl)
	ld h, b
	ld l, c
	ld b, a

SafeVmemcpyLoop:
	call _gbc_sync_di

SafeVmemcpyStatLoop:
	ld a, (_STAT_REG + 0)
#ifdef __POCKET__
	bit 6, a
#else
	bit 1, a
#endif
	jr nz, SafeVmemcpyStatLoop

	ld a, (hl+)
	ld (de), a
	inc de
	ld a, (hl+)
	ld (de), a
	ei
	inc de

	dec b
	jr nz,SafeVmemcpyLoop
__endasm;
}

static void gbc_text_remove_color(uint8_t y, uint8_t col) OLDCALL {
__asm
	ldhl sp, #2
	; ld a, (hl)
	; opt1: lower code uses ldhl sp, #3
	ld a, (hl+)

	ld e, a
	ld a, (_draw_offset_y)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	xor a, a
	sra d
	rra
	ld e, a

	; a = col
	; ldhl sp, #3
	; opt1: see above
	ld a, (hl)

	; de = y << 7 | col
 	add a, e
	ld e, a

	; de |= 0xD000
	ld a, d
	or a, #0xD0
	ld d, a

	; bc = y << 7
	ld a, d
	and a, #0x0F
	ld b, a
	ld a, e
	and a, #0x80

	; bc = y << 4
.rept 3
	sra b
	rra
.endm
	ld c, a

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 3
	ld a, #0x03
	ldh (_SVBK_REG + 0), a

	; read [de]
	ld a, (de)

	; if palette color map entry unused, return
	cp a, #0xFF
	jr z, GbcTextRemoveColorFinish

	; bc |= entry
	; bc = 0xD000 | y << 4 | entry
	or a, c
	ld c, a

	; set SVBK to 4
	ld a, #0x04
	ldh (_SVBK_REG + 0), a

	; read [bc]
	ld a, (bc)

	; not allocated?
	cp a, #0x00
	jr z, GbcTextRemoveColorFinish

	; deallocate (palette usage map)
	dec a
	ld (bc), a
	jr nz, GbcTextRemoveColorFinish

	; set SVBK to 3
	ld a, #0x03
	ldh (_SVBK_REG + 0), a

	; deallocate (palette color map)
	ld a, #0xFF
	ld (de), a

GbcTextRemoveColorFinish:
	; clear SVBK
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

static void gbc_text_free_line(uint8_t y) {
__asm
	ld e, a
	ld a, (_draw_offset_y)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	xor a, a
	sra d
	rra
	ld e, a

	; bc = y << 7
	ld b, d
	ld c, e

	; de |= 0xD000
	ld a, d
	or a, #0xD0
	ld d, a

	; bc = y << 4
	ld a, c
.rept 3
	sra b
	rra
.endm
	ld c, a

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	ld h, #0x10 ; steps of 0x08
GbcTextFreeLineLoop1:
	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	ld a, #0x03
	ldh (_SVBK_REG + 0), a

	ld a, #0xFF
.rept 8
	ld (de), a
	inc de
.endm

	xor a, a
	ldh (_SVBK_REG + 0), a
	ei

	dec h
	jr nz, GbcTextFreeLineLoop1

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 4
	ld a, #0x04
	ldh (_SVBK_REG + 0), a

	xor a, a
	ld h, #0x10

GbcTextFreeLineLoop2:
	ld (bc), a
	inc bc
	dec h
	jr nz, GbcTextFreeLineLoop2

	; clear SVBK
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

static uint8_t gbc_text_add_color(uint8_t y, uint8_t col) OLDCALL {
__asm
	ldhl sp, #2
	; ld a, (hl)
	; opt1: lower code uses ldhl sp, #3
	ld a, (hl+)

	ld e, a
	ld a, (_draw_offset_y)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	xor a, a
	sra d
	rra
	ld e, a

	; a = col
	; ldhl sp, #3
	; opt1: see above
	ld a, (hl)

	; de = y << 7 | col
	add a, e
	ld e, a

	; de |= 0xD000
	ld a, d
	or a, #0xD0
	ld d, a

	; bc = y << 7
	ld a, d
	and a, #0x0F
	ld b, a
	ld a, e
	and a, #0x80

	; bc = y << 4
.rept 3
	sra b
	rra
.endm
	ld c, a

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 3
	ld a, #0x03
	ldh (_SVBK_REG + 0), a

	; read [de]
	ld a, (de)
	cp a, #0xFF
	jr z, GbcTextAddColorAlloc

	; bc |= entry
	; bc = 0xD000 | y << 4 | entry
	or a, c
	ld c, a

	; set SVBK to 4
	ld a, #0x04
	ldh (_SVBK_REG + 0), a

	; read [bc]
	ld a, (bc)
	jp GbcTextAddColorIncrAlloc

GbcTextAddColorAlloc:
	; set SVBK to 4
	ld a, #0x04
	ldh (_SVBK_REG + 0), a

	ld h, #0x00
	; color not present, allocate...
	ld a, (bc)
	cp a, h
	jr z, GbcTextAddColorAllocFound
.rept 9
	inc bc
	ld a, (bc)
	cp a, h
	jr z, GbcTextAddColorAllocFound
.endm

	; TODO: better handle the "color not found" scenario

	; clear SVBK
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	; return zero
	ld e, a
	ret

GbcTextAddColorAllocFound:
	; set SVBK to 3
	ld a, #0x03
	ldh (_SVBK_REG + 0), a

	; c & 0x0F = slot
	ld a, c
	and a, #0x0F
	ld (de), a

	; we only need to preserve (bc) from here
	; but we need to set the colors in bank 2
	; the goal is de = y << 6 | slot << 2

	; set SVBK to 0 so we can access the y/col values again
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	ldhl sp, #2
	; ld a, (hl)
	; opt1: lower code uses ldhl sp, #3
	ld a, (hl+)

	ld e, a
	ld a, (_draw_offset_y)
	add a, e
	and a, #0x1F

	; de = y << 6
	ld d, a
	xor a, a
	sra d
	rra
	sra d
	rra
	ld e, a

	; de = y << 6 | slot << 2
	ld a, c
	and a, #0x0F
	sla a
	sla a
	or a, e
	ld e, a

	; de |= 0xD000
	ld a, d
	or a, #0xD0
	ld d, a

	; a = color
	; ldhl sp, #3
	; opt1: see above
	ld a, (hl)
	ld (0xFFA3), a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 2
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; calculate offset for bg color
	ld hl, #_cgb_palette
	ld a, (0xFFA3)
	swap a
	and a, #0x0F
	sla a

	add a, l
	ld l, a
#ifndef CGB_PALETTE_IS_PAGE_ALIGNED
	ld a, h
	adc a, #0
	ld h, a
#endif

	; copy data
	ld a, (hl+)
	ld (de), a
	inc de
	ld a, (hl)
	ld (de), a
	inc de

	; calculate offset for fg color
	ld hl, #_cgb_palette
	ld a, (0xFFA3)
	and a, #0x0F
	sla a

	add a, l
	ld l, a
#ifndef CGB_PALETTE_IS_PAGE_ALIGNED
	ld a, h
	adc a, #0
	ld h, a
#endif

	; copy data
	ld a, (hl+)
	ld (de), a
	inc de
	ld a, (hl)
	ld (de), a

	; set SVBK to 4
	ld a, #0x04
	ldh (_SVBK_REG + 0), a

	; set usage count to 0
	xor a, a
GbcTextAddColorIncrAlloc:
	inc a
	ld (bc), a

GbcTextAddColorFinish:
	; clear SVBK
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	; c & 0x0F - palette entry
	ld a, c
	and a, #0x0F
	ld e, a
__endasm;
}

static void gbc_text_undraw(uint8_t x, uint8_t y) {
__asm
	ld d, a
	; de = xy

	; create X/Y pointer
	ld hl, #(_draw_offset_x)
	ld a, (hl+)
	ld b, (hl)
	ld c, a
	; b = draw_offset_y, c = draw_offset_x

	ld a, d
	add a, c
	and a, #0x1F ; a = (x + draw_offset_x) & 0x1F
	ld (0xFFA3), a

	ld a, e
	ld (0xFFA4), a
	add a, b
	and a, #0x1F ; a = (y + draw_offset_y) & 0x1F
	ld d, a
	xor a, a
.rept 3
	sra d
	rra
.endm
	ld e, a

	ld a, (0xFFA3)
	or a, e ; da = X/Y pointer
	ld e, a ; de = X/Y pointer too

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	ld a, d
	or a, #0xD8
	ld d, a

	; read color
	ld a, (de)
	ld d, a

	; clear SVBK
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	ld a, (0xFFA4)
	ld e, a

	; de = col, y
	push bc
	push de
	call _gbc_text_remove_color
	pop de
	pop bc
__endasm;
}

static void gbc_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) OLDCALL {
__asm
	; create X/Y pointer
	ld hl, #(_draw_offset_x)
	ld a, (hl+)
	ld b, (hl)
	ld c, a
	; b = draw_offset_y, c = draw_offset_x
	ldhl sp, #3
	ld a, (hl-)
	add a, b
	and a, #0x1F ; a = (y + draw_offset_y) & 0x1F
	ld d, a
	xor a, a
.rept 3
	sra d
	rra
.endm
	ld e, a
	ld a, (hl)
	add a, c
	and a, #0x1F ; a = (x + draw_offset_x) & 0x1F
	or a, e ; da = X/Y pointer
	ld e, a ; de = X/Y pointer too

	; 0xFFA0 - 0xFFA3 = X/Y pointer, chr, col
	ld (0xFFA0), a
	ld a, d
	ld (0xFFA1), a
	ldhl sp, #4
	ld a, (hl+)
	ld (0xFFA2), a
	ld a, (hl)
	and a, #0x7F ; blinking not supported
	ld (0xFFA3), a

	; get existing tile color
	ld b, a ; last value saved above is col

	ld a, d
	or a, #0xD8
	ld d, a

	; renderer scrolling check 1 - assume color changed
	ld a, (_renderer_scrolling)
	bit 0, a
	jr nz, GbcTextDrawColorChanged

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	ld a, (de)
	ld c, a

	xor a, a
	ldh (_SVBK_REG + 0), a
	ei

	ld a, c
	cp a, b
	jr z, GbcTextDrawSetChar

GbcTextDrawColorChanged:
	; color changed
	; a = old tile color
	; b = new tile color

	ld c, a ; bc = new, old

	ldhl sp, #3
	ld a, (hl)
	ld e, a

	; renderer scrolling check 2 - no remove
	ld a, (_renderer_scrolling)
	bit 0, a
	jr nz, GbcTextDrawNoRemove

	ld d, c ; de = old, y
	push bc
	push de
	call _gbc_text_remove_color
	pop de
	pop bc
GbcTextDrawNoRemove:
	ld d, b ; de = new, y
	push de
	call _gbc_text_add_color
	ld a, e
	ld (0xFFA4), a
	pop de

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; create buffer pointer for color
	ld a, (0xFFA0)
	ld e, a
	ld a, (0xFFA1)
	or a, #0xD8
	ld d, a

	; write color
	ld a, (0xFFA3)
	ld (de), a

	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	; prepare value to write to VRAM bank 1
	ld a, (0xFFA4)
	rra
	jr nc, GbcTextDrawSetColorAAA
	or a, #0x08

GbcTextDrawSetColorAAA:
	ld b, a
	ld a, d
	and a, #0xBB ; 0xD8 -> 0x98
	ld d, a

	ld a, #0x01
	ldh (_VBK_REG + 0), a

	ld hl, #(_STAT_REG)
	call _gbc_sync_di
	ld a, b
GbcTextDrawSetColorLoop:
#ifdef __POCKET__
	bit 6, (hl)
#else
	bit 1, (hl)
#endif
	jr nz, GbcTextDrawSetColorLoop

	ld (de), a
	ei

	xor a, a
	ldh (_VBK_REG + 0), a

	; all registers trashed

GbcTextDrawSetChar:
	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ldh (_SVBK_REG + 0), a

	; read char from buffer
	ld a, (0xFFA0)
	ld e, a
	ld a, (0xFFA1)
	or a, #0xDC
	ld d, a
	ld a, (de)
	ld b, a
	ld a, (0xFFA2)

	cp a, b
	jr z, GbcTextDrawFinish

	ld (de), a
	ld b, a

	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts

	ld a, d
	and a, #0xBB ; 0xDC -> 0x98
	ld d, a

	ld hl, #(_STAT_REG)
	call _gbc_sync_di
	ld a, b
GbcTextDrawSetCharLoop:
#ifdef __POCKET__
	bit 6, (hl)
#else
	bit 1, (hl)
#endif
	jr nz, GbcTextDrawSetCharLoop

	ld (de), a
	ei

GbcTextDrawFinish:
	; clear SVBK
	xor a, a
	ldh (_SVBK_REG + 0), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

// TODO (SDCC 4.2 upgrade) - remove
static void gbc_text_draw_wrap(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	gbc_text_draw(x, y, chr, col);
}

void gbc_text_init(uint8_t mode);

static void gbc_text_update(void) {
	scx_shadow_reg = draw_offset_x << 3;
	scy_shadow_reg = draw_offset_y << 3;
}

static void gbc_text_scroll(int8_t dx, int8_t dy) {
	draw_offset_x = (draw_offset_x + dx) & 0x1F;
	draw_offset_y = (draw_offset_y + dy) & 0x1F;
}

const renderer_t renderer_gbc = {
	gbc_text_init,
	gbc_text_sync_hblank_safe,
	gbc_text_undraw,
	gbc_text_draw_wrap,
	gbc_text_free_line,
	gbc_text_scroll,
	gbc_text_update
};

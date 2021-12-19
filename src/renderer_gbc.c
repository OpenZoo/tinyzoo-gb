#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "renderer.h"
#include "font_manager.h"

#define COL_LVL_0 0
#define COL_LVL_1 10
#define COL_LVL_2 21
#define COL_LVL_3 31

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

uint16_t cgb_message_palette[18];

static uint16_t hblank_isr_sp;
static uint16_t hblank_isr_pal_pos;
extern uint8_t ly_bank_switch;
static uint8_t new_lcdc_val;
static uint8_t ly_bank_switch_mirror;

static void vblank_update_palette(void) {
__asm
	push bc

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

	pop bc
__endasm;
}

static void gbc_hblank_switch_window(void) __naked {
__asm
.hblank_switch_window_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
	bit 1, a				; 1 cycles
	jr nz, .hblank_switch_window_sync		; 1.5 cycles

	ld a, #0xC9 ; 8
	ldh (_LCDC_REG + 0), a ; 12

	pop af
	reti
__endasm;
}

static void hblank_update_palette(void) __naked {
__asm
	push hl
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
	ld a, (_ly_bank_switch_mirror)
	ld b, a
	ldh a, (_LYC_REG + 0)
	cp a, b
	jr nc, .hblank_update_palette_window
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
	jr nz, .hblank_update_palette_sync		; 1.5 cycles

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

.hblank_update_palette_restore:
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

.hblank_update_palette_window:
	; prepare palette register
	ld a, #0x80
	ldh (_BCPS_REG + 0), a

	ld hl, #(_cgb_message_palette)

	; set sp for stack copy; hl = BCPD
	ld sp, hl
	ld hl, #(_BCPD_REG)

	; preload values (timing is tight)
	pop bc
	pop de

	; wait for STAT to be correct
.hblank_update_palette_window_sync:
	ldh a, (_STAT_REG + 0)	; 1.5 cycles
	bit 1, a				; 1 cycles
	jr nz, .hblank_update_palette_window_sync		; 1.5 cycles

	; budget: 67-71 cycles
	ld (hl), c	; 1 cycle
	ld (hl), b	; 1 cycle
	ld (hl), e	; 1 cycle
	ld (hl), d	; 1 cycle
.rept 14
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

	LCDC_REG = 0b11010001;
	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;

	hblank_isr_ip = (uint16_t) hblank_update_palette;
	hblank_isr_pal_pos = 0xD000 | ((uint16_t)local_doy << 6);
	LYC_REG = 7;
	ly_bank_switch_mirror = ly_bank_switch;
	new_lcdc_val = (ly_bank_switch < 135) ? 0xD9 : 0xC9;

	vblank_update_palette();
	global_vblank_isr();
}

static void gbc_sync_di(void) __naked __preserves_regs(d, e, h, l) {
__asm;
	ldh a, (_LY_REG + 0)
	cp a, #135
	jr nc, GbcSyncDiDone
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
static void gbc_text_sync_hblank_safe(void) __naked {
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

static void gbc_text_remove_color(uint8_t y, uint8_t col) {
__asm
	ldhl sp, #2
	ld a, (hl)

	ld e, a
	ld hl, #(_draw_offset_y)
	ld a, (hl)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	ld e, #0
	sra d
	rr e

	; a = col
	ldhl sp, #3
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
	ld c, a

	; bc = y << 4
.rept 3
	sra b
	rr c
.endm

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 3
	ld a, #0x03
	ld (_SVBK_REG), a

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
	ld (_SVBK_REG), a

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
	ld (_SVBK_REG), a

	; deallocate (palette color map)
	ld a, #0xFF
	ld (de), a

GbcTextRemoveColorFinish:
	; clear SVBK
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

static void gbc_text_free_line(uint8_t y) {
__asm
	ldhl sp, #2
	ld a, (hl)

	ld e, a
	ld hl, #(_draw_offset_y)
	ld a, (hl)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	ld e, #0
	sra d
	rr e

	; bc = y << 7
	ld b, d
	ld c, e

	; de |= 0xD000
	ld a, d
	or a, #0xD0
	ld d, a

	; bc = y << 4
.rept 3
	sra b
	rr c
.endm

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	ld h, #0x10 ; steps of 0x08
GbcTextFreeLineLoop1:
	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	ld a, #0x03
	ld (_SVBK_REG), a

	ld a, #0xFF
.rept 8
	ld (de), a
	inc de
.endm

	ld a, #0x00
	ld (_SVBK_REG), a
	ei

	dec h
	jr nz, GbcTextFreeLineLoop1

	; set SVBK to 4
	ld a, #0x04
	ld (_SVBK_REG), a

	ld a, #0x00
	ld h, #0x10

GbcTextFreeLineLoop2:
	ld (bc), a
	inc bc
	dec h
	jr nz, GbcTextFreeLineLoop2

	; clear SVBK
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

static uint8_t gbc_text_add_color(uint8_t y, uint8_t col) {
__asm
	ldhl sp, #2
	ld a, (hl)

	ld e, a
	ld hl, #(_draw_offset_y)
	ld a, (hl)
	add a, e
	and a, #0x1F

	; de = y << 7
	ld d, a
	ld e, #0
	sra d
	rr e

	; a = col
	ldhl sp, #3
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
	ld c, a

	; bc = y << 4
.rept 3
	sra b
	rr c
.endm

	; bc |= 0xD000
	ld a, b
	or a, #0xD0
	ld b, a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 3
	ld a, #0x03
	ld (_SVBK_REG), a

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
	ld (_SVBK_REG), a

	; read [bc]
	ld a, (bc)
	jp GbcTextAddColorIncrAlloc

GbcTextAddColorAlloc:
	; set SVBK to 4
	ld a, #0x04
	ld (_SVBK_REG), a

	ld h, #0x00
	; color not present, allocate...
	ld a, (bc)
	cp a, h
	jr z, GbcTextAddColorAllocFound
.rept 8
	inc bc
	ld a, (bc)
	cp a, h
	jr z, GbcTextAddColorAllocFound
.endm

	; TODO: better handle the "color not found" scenario

	; clear SVBK
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	; return zero
	ld e, a
	ret

GbcTextAddColorAllocFound:
	; set SVBK to 3
	ld a, #0x03
	ld (_SVBK_REG), a

	; c & 0x0F = slot
	ld a, c
	and a, #0x0F
	ld (de), a

	; we only need to preserve (bc) from here
	; but we need to set the colors in bank 2
	; the goal is de = y << 6 | slot << 2

	; set SVBK to 0 so we can access the y/col values again
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	ldhl sp, #2
	ld a, (hl)

	ld e, a
	ld hl, #(_draw_offset_y)
	ld a, (hl)
	add a, e
	and a, #0x1F

	; de = y << 6
	ld d, a
	ld e, #0
	sra d
	rr e
	sra d
	rr e

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
	ldhl sp, #3
	ld a, (hl)
	ld (0xFFA3), a

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; set SVBK to 2
	ld a, #0x02
	ld (_SVBK_REG), a

	; calculate offset for bg color
	ld hl, #_cgb_palette
	ld a, (0xFFA3)
	swap a
	and a, #0x0F
	sla a

	add a, l
	ld l, a
	ld a, h
	adc a, #0
	ld h, a

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
	ld a, h
	adc a, #0
	ld h, a

	; copy data
	ld a, (hl+)
	ld (de), a
	inc de
	ld a, (hl)
	ld (de), a

	; set SVBK to 4
	ld a, #0x04
	ld (_SVBK_REG), a

	; set usage count to 0
	xor a, a
GbcTextAddColorIncrAlloc:
	inc a
	ld (bc), a

GbcTextAddColorFinish:
	; clear SVBK
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	; c & 0x0F - palette entry
	ld a, c
	and a, #0x0F
	ld e, a
__endasm;
}

static void gbc_text_undraw(uint8_t x, uint8_t y) {
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
	ld e, a
	ld d, #0x00
.rept 5
	sla e
	rl d
.endm
	ld a, (hl)
	add a, c
	and a, #0x1F ; a = (x + draw_offset_x) & 0x1F
	or a, e ; da = X/Y pointer
	ld e, a ; de = X/Y pointer too

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ld (_SVBK_REG), a

	ld a, d
	or a, #0xD8
	ld d, a

	; read color
	ld a, (de)
	ld d, a

	; clear SVBK
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	ldhl sp, #3
	ld a, (hl)
	ld e, a
	; de = col, y
	push bc
	push de
	call _gbc_text_remove_color
	pop de
	pop bc
__endasm;
}

static void gbc_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
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
	ld e, a
	ld d, #0x00
.rept 5
	sla e
	rl d
.endm
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
	ld hl, #(_renderer_scrolling)
	ld a, (hl)
	bit 0, a
	jr nz, GbcTextDrawColorChanged

	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ld (_SVBK_REG), a

	ld a, (de)

	xor a, a
	ld (_SVBK_REG), a
	ei

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
	ld hl, #(_renderer_scrolling)
	ld a, (hl)
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
	ld (_SVBK_REG), a

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
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	; prepare value to write to VRAM bank 1
	ld a, (0xFFA4)
	rr a
	jr nc, GbcTextDrawSetColorAAA
	or a, #0x08

GbcTextDrawSetColorAAA:
	ld b, a
	ld a, d
	and a, #0xBB ; 0xD8 -> 0x98
	ld d, a

	ld a, #0x01
	ld (_VBK_REG), a

	ld hl, #(_STAT_REG)
	call _gbc_sync_di
	ld a, b
GbcTextDrawSetColorLoop:
	bit 1, (hl)
	jr nz, GbcTextDrawSetColorLoop

	ld (de), a
	ei

	xor a, a
	ld (_VBK_REG), a

	; all registers trashed

GbcTextDrawSetChar:
	call _gbc_sync_di ; SVBK cannot be changed between interrupts
	; configure SVBK
	ld a, #0x02
	ld (_SVBK_REG), a

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
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts

	ld a, d
	and a, #0xBB ; 0xDC -> 0x98
	ld d, a

	ld hl, #(_STAT_REG)
	call _gbc_sync_di
	ld a, b
GbcTextDrawSetCharLoop:
	bit 1, (hl)
	jr nz, GbcTextDrawSetCharLoop

	ld (de), a
	ei

GbcTextDrawFinish:
	; clear SVBK
	xor a, a
	ld (_SVBK_REG), a
	ei ; SVBK cannot be changed between interrupts
__endasm;
}

void gbc_text_init(void);

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
	gbc_text_draw,
	gbc_text_free_line,
	gbc_text_scroll,
	gbc_text_update
};

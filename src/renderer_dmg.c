#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"
#include "../res/font_default.h"

static void vblank_isr(void) {
	SCX_REG = scx_shadow_reg;
	SCY_REG = scy_shadow_reg;
}

static void dmg_text_init(void) {
	volatile uint8_t *vptr = (uint8_t*) 0x8000;
	const uint8_t *fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		while (STAT_REG & 0x02);
		*(vptr++) = *fptr;
		*(vptr++) = *fptr;
	}

	vblank_isr();
	add_VBL(vblank_isr);

	BGP_REG = 0b11001100;
}

static void dmg_text_draw(uint8_t x, uint8_t y, uint8_t chr, uint8_t col) {
	x = (draw_offset_x + x) & 0x1F;
	y = (draw_offset_y + y) & 0x1F;
	
	volatile uint8_t *tptr = (uint8_t*) (0x9800 | (y << 5) | x);
	while (STAT_REG & 0x02);
	*tptr = chr;
}

static void dmg_text_mark_redraw(void) {

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
	dmg_text_draw,
	dmg_text_mark_redraw,
	dmg_text_scroll,
	dmg_text_update
};
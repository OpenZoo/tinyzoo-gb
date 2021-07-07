#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer.h"

renderer_t active_renderer;

uint8_t draw_offset_x = 0;
uint8_t draw_offset_y = 0;
uint8_t scx_shadow_reg = 0;
uint8_t scy_shadow_reg = 0;

void text_init(const renderer_t *renderer) {
	LCDC_REG = 0x00;
	hblank_isr_jp = 0xC3;

	memcpy(&active_renderer, renderer, sizeof(renderer_t));
	active_renderer.init();

	wait_vbl_done();
	LCDC_REG = 0b11010001;
}

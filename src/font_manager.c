#pragma bank 3

#include <stdint.h>
#include <string.h>
#include "font_manager.h"
#include "../res/font_default.h"

void font_8x8_install(uint8_t color_bg, uint8_t color_fg) BANKED {
	// TODO: Rewrite to ASM, optimize

	uint8_t mask_bg_1 = ((color_bg & 1) ? 0xFF : 0x00);
	uint8_t mask_bg_2 = ((color_bg & 2) ? 0xFF : 0x00);
	uint8_t mask_fg_1 = ((color_fg & 1) ? 0xFF : 0x00);
	uint8_t mask_fg_2 = ((color_fg & 2) ? 0xFF : 0x00);

	volatile uint8_t *vptr = (uint8_t*) 0x8000;
	const uint8_t *fptr = _font_default_bin;
	for (uint16_t i = 0; i < 2048; i++, fptr++) {
		uint8_t vf = *fptr;
		uint8_t v1 = (mask_bg_1 & (~vf)) | (mask_fg_1 & (vf));
		uint8_t v2 = (mask_bg_2 & (~vf)) | (mask_fg_2 & (vf));
		*(vptr++) = v1;
		*(vptr++) = v2;
	}
}
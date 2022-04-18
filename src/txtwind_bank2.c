#pragma bank 2

#include <gb/gb.h>
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "txtwind.h"

uint16_t txtwind_lines;

void txtwind_init(void) BANKED {
	txtwind_lines = 0;
}

void txtwind_append(uint16_t line_ptr, uint8_t line_bank) BANKED {
	if (txtwind_lines > MAX_TEXT_WINDOW_LINES) {
		return;
	}

	sram_ptr_t ptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (txtwind_lines * 3);

	ENABLE_RAM_MBC5;
	sram_write8(&ptr, line_ptr);
	sram_write8(&ptr, line_ptr >> 8);
	sram_write8(&ptr, line_bank);
	DISABLE_RAM_MBC5;

	txtwind_lines++;
}

static void txtwind_draw_line(uint16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);
	uint8_t pal_color = 0;
	if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
		pal_color = 8;
	} else if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		pal_color = 1; // TODO
	}

	VBK_REG = 1;
	fill_bkg_rect(0, idx & 31, 20, 1, pal_color);
	VBK_REG = 0;
	if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
		fill_bkg_rect(0, idx & 31, 20, 1, 32);
		set_bkg_tiles((20 - line.len) >> 1, idx & 31, line.len, 1, line.text);
	} else {
		set_bkg_tiles(0, idx & 31, line.len, 1, line.text);
		fill_bkg_rect(line.len, idx & 31, 20 - line.len, 1, 32);
	}
}

void txtwind_draw(void) BANKED {
	uint8_t i;
	uint8_t pos = 0;

	text_reinit(RENDER_MODE_TXTWIND);
	for (i = 0; i < 20; i++) {
		txtwind_draw_line(i);
	}

	while (true) {
		wait_vbl_done();
		uint8_t keys = joypad();
		if (keys & J_UP) {
			if (pos > 0) {
				pos--;
				txtwind_draw_line(pos);
			}
		} else if (keys & J_DOWN) {
			if (pos < (txtwind_lines - 17)) {
				txtwind_draw_line(pos + 18);
				pos++;
			}
		} else if (keys & J_A) {
			// TODO: select
			break;
		} else if (keys & J_B) {
			break;
		}
		draw_offset_y = pos & 31;
		text_update();
	}

	text_reinit(RENDER_MODE_PLAYFIELD);
	board_redraw();
	game_update_sidebar_all();
}

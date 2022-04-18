#pragma bank 2

#include <gb/gb.h>
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "oop.h"
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

static void txtwind_draw_line(int16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);
	uint8_t pal_color = 0;
	uint8_t offset = 1;

	if ((idx == -1) || (idx == txtwind_lines)) {
		if (renderer_id == RENDER_ID_GBC) {
			VBK_REG = 1;
			fill_win_rect(0, idx & 31, 20, 1, 8);
			VBK_REG = 0;
		}
		fill_win_rect(0, idx & 31, 20, 1, 196);
		return;
	} else {
		fill_win_rect(0, idx & 31, 20, 1, 32);
		fill_win_rect(19, idx & 31, 20, 1, 32);
	}

	if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
		pal_color = 8;
	} else if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		pal_color = 8;
		offset = 4;
	}

	if (renderer_id == RENDER_ID_GBC) {
		VBK_REG = 1;
		fill_win_rect(1, idx & 31, 18, 1, pal_color);
		VBK_REG = 0;
	}
	fill_win_rect(1, idx & 31, 18, 1, 32);

	if (line.len > 0) {
		if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
			set_win_tiles((20 - line.len) >> 1, idx & 31, line.len, 1, line.text);
		} else {
			set_win_tiles(offset, idx & 31, line.len, 1, line.text);
		}
	}

	if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		if (renderer_id == RENDER_ID_GBC) {
			VBK_REG = 1;
			fill_win_rect(2, idx & 31, 1, 1, 1);
			VBK_REG = 0;
		}
		fill_win_rect(2, idx & 31, 1, 1, 16);
	}
}

static bool txtwind_exec_line(uint16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);

	if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		if (oop_send_target(line.target_id, false, line.label_id, false)) {
			return true;
		}
	}

	return false;
}

bool txtwind_run(void) BANKED OLDCALL {
	uint8_t old_draw_offset_x = draw_offset_x;
	uint8_t old_draw_offset_y = draw_offset_y;

	uint8_t i;
	int16_t old_pos = -8;
	int16_t pos = -8;
	bool result = false;

	wait_vbl_done();
	text_reinit(RENDER_MODE_TXTWIND);
	draw_offset_y = pos & 31;
	text_update();

	if (renderer_id == RENDER_ID_GBC) {
		VBK_REG = 1;
		fill_win_rect(0, 0, 1, 32, 2);
		fill_win_rect(19, 0, 1, 32, 2);
		VBK_REG = 0;
	}
	fill_win_rect(0, 0, 1, 32, 32);
	fill_win_rect(19, 0, 1, 32, 32);

	for (i = 0; i < 18; i++) {
		txtwind_draw_line(pos + i);
	}

	uint8_t keys;
	while (true) {
		wait_vbl_done();
		wait_vbl_done();
		keys = joypad();

		if (keys & J_UP) {
			if (pos > -8) {
				pos--;
				txtwind_draw_line(pos);
			}
		} else if (keys & J_DOWN) {
			if (pos < (((int16_t) txtwind_lines) - 9)) {
				txtwind_draw_line(pos + 18);
				pos++;
			}
		} else if (keys & J_A) {
			result = txtwind_exec_line(pos + 8);
			break;
		} else if (keys & J_B) {
			break;
		}

		draw_offset_y = pos & 31;
		text_update();
		wait_vbl_done();

		VBK_REG = 0;
		if (old_pos != -9 && old_pos != (txtwind_lines - 8)) {
			fill_win_rect(0, (old_pos + 8) & 31, 1, 1, 32);
			fill_win_rect(19, (old_pos + 8) & 31, 1, 1, 32);
		}
		old_pos = pos;
		if (pos != -9 && pos != (txtwind_lines - 8)) {
			if (renderer_id == RENDER_ID_GBC) {
				VBK_REG = 1;
				fill_win_rect(0, (draw_offset_y + 8) & 31, 1, 1, 2);
				fill_win_rect(19, (draw_offset_y + 8) & 31, 1, 1, 2);
				VBK_REG = 0;
			}
			fill_win_rect(0, (draw_offset_y + 8) & 31, 1, 1, 175);
			fill_win_rect(19, (draw_offset_y + 8) & 31, 1, 1, 174);
		}
	}

	wait_vbl_done();
	text_reinit(RENDER_MODE_PLAYFIELD);
	draw_offset_x = old_draw_offset_x;
	draw_offset_y = old_draw_offset_y;
	text_update();
	game_update_sidebar_all();
	return result;
}

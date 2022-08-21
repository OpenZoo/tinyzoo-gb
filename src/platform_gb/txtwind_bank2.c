#pragma bank 2

#include <gbdk/platform.h>

#include "bank_switch.h"
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "oop.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "txtwind.h"

#define TEXT_WIDTH_INNER 19
#define TEXT_WIDTH_OUTER (TEXT_WIDTH_INNER + 2)

extern uint16_t txtwind_lines;
void gbc_init_set_palette(void) BANKED;
void gbc_init_unset_palette(void) BANKED;

static void txtwind_draw_line_txtwind(int16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);
	uint8_t pal_color = 0;
	uint8_t offset = 1;

	IE_REG &= ~TIM_IFLAG;

	if ((idx == -1) || (idx == txtwind_lines)) {
		if (renderer_id == RENDER_ID_GBC) {
			VBK_REG = 1;
			fill_win_rect(0, idx & 31, TEXT_WIDTH_OUTER, 1, 8);
			VBK_REG = 0;
		}
		fill_win_rect(0, idx & 31, TEXT_WIDTH_OUTER, 1, 196);
		return;
	} else {
		fill_win_rect(0, idx & 31, TEXT_WIDTH_OUTER, 1, 32);
		fill_win_rect(TEXT_WIDTH_OUTER - 1, idx & 31, TEXT_WIDTH_OUTER, 1, 32);
	}

	if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
		pal_color = 8;
	} else if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		pal_color = 8;
		offset = 4;
	}

	if (renderer_id == RENDER_ID_GBC) {
		VBK_REG = 1;
		fill_win_rect(1, idx & 31, TEXT_WIDTH_INNER, 1, pal_color);
		VBK_REG = 0;
	}
	fill_win_rect(1, idx & 31, TEXT_WIDTH_INNER, 1, 32);

	if (line.len > 0) {
		if (line.type == TXTWIND_LINE_TYPE_CENTERED) {
			set_win_tiles((TEXT_WIDTH_OUTER - line.len) >> 1, idx & 31, line.len, 1, line.text);
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

	IE_REG |= TIM_IFLAG;
}

bool txtwind_exec_line(uint16_t idx);

static uint8_t txtwind_run_txtwind(void) {
	uint8_t i;
	int16_t old_pos = -8;
	int16_t pos = -8;
	uint8_t result = 255;

	draw_offset_y = pos & 31;
	text_update();

	scx_shadow_reg = 4;

	IE_REG &= ~TIM_IFLAG;

	if (renderer_id == RENDER_ID_GBC) {
		VBK_REG = 1;
		fill_win_rect(0, 0, 1, 32, 2);
		fill_win_rect(TEXT_WIDTH_OUTER - 1, 0, 1, 32, 2);
		VBK_REG = 0;
	}
	fill_win_rect(0, 0, 1, 32, 32);
	fill_win_rect(TEXT_WIDTH_OUTER - 1, 0, 1, 32, 32);

	IE_REG |= TIM_IFLAG;

	for (i = 0; i < 18; i++) {
		txtwind_draw_line_txtwind(pos + i);
	}

	wait_vbl_done();
	gbc_init_set_palette();

	uint8_t keys;
	while (true) {
		wait_vbl_done();
		wait_vbl_done();
		keys = joypad();

		if (keys & J_UP) {
			if (pos > -8) {
				pos--;
				txtwind_draw_line_txtwind(pos);
			}
		} else if (keys & J_DOWN) {
			if (pos < (((int16_t) txtwind_lines) - 9)) {
				txtwind_draw_line_txtwind(pos + 18);
				pos++;
			}
		} else if (keys & J_A) {
			result = txtwind_exec_line(pos + 8) ? 0 : 255;
			break;
		} else if (keys & J_B) {
			break;
		}

		draw_offset_y = pos & 31;
		text_update();
		scx_shadow_reg = 4;
		wait_vbl_done();

		IE_REG &= ~TIM_IFLAG;

		VBK_REG = 0;
		if (old_pos != -9 && old_pos != (txtwind_lines - 8)) {
			fill_win_rect(0, (old_pos + 8) & 31, 1, 1, 32);
			fill_win_rect(TEXT_WIDTH_OUTER - 1, (old_pos + 8) & 31, 1, 1, 32);
		}
		old_pos = pos;
		if (pos != -9 && pos != (txtwind_lines - 8)) {
			if (renderer_id == RENDER_ID_GBC) {
				VBK_REG = 1;
				fill_win_rect(0, (draw_offset_y + 8) & 31, 1, 1, 2);
				fill_win_rect(TEXT_WIDTH_OUTER - 1, (draw_offset_y + 8) & 31, 1, 1, 2);
				VBK_REG = 0;
			}
			fill_win_rect(0, (draw_offset_y + 8) & 31, 1, 1, 175);
			fill_win_rect(TEXT_WIDTH_OUTER - 1, (draw_offset_y + 8) & 31, 1, 1, 174);
		}

		IE_REG |= TIM_IFLAG;
	}

	while (keys != 0) {
		wait_vbl_done();
		keys = joypad();
	}

	return result;
}

static uint8_t txtwind_run_menu(void) {
	txtwind_line_t line;
	uint8_t wnd_width = 0;
	uint8_t wnd_height = txtwind_lines;
	uint8_t result = 255;

	draw_offset_x = 0;
	draw_offset_y = 0;
	text_update();

	for (uint8_t idx = 0; idx < wnd_height; idx++) {
		txtwind_read_line(idx, &line);
		if (wnd_width < line.len) {
			wnd_width = line.len;
		}
	}

	uint8_t wnd_x = (20 - (wnd_width + 6)) >> 1;
	uint8_t wnd_y = (18 - (wnd_height + 4)) >> 1;
	uint8_t arrow_y = 0;

	// clear + colors
	IE_REG &= ~TIM_IFLAG;

	if (renderer_id == RENDER_ID_GBC) {
		VBK_REG = 1;
		fill_win_rect(0, 0, 20, 18, 0);
		fill_win_rect(wnd_x + 2, wnd_y + 2, 1, wnd_height, 8);
		VBK_REG = 0;
	}
	fill_win_rect(0, 0, 20, 18, 32);

	// border
	fill_win_rect(wnd_x + 1, wnd_y, wnd_width + 4, 1, 205);
	fill_win_rect(wnd_x + 1, wnd_y + wnd_height + 3, wnd_width + 4, 1, 205);
	fill_win_rect(wnd_x, wnd_y + 1, 1, wnd_height + 2, 186);
	fill_win_rect(wnd_x + wnd_width + 5, wnd_y + 1, 1, wnd_height + 2, 186);
	set_win_tile_xy(wnd_x, wnd_y, 201);
	set_win_tile_xy(wnd_x + wnd_width + 5, wnd_y, 187);
	set_win_tile_xy(wnd_x, wnd_y + wnd_height + 3, 200);
	set_win_tile_xy(wnd_x + wnd_width + 5, wnd_y + wnd_height + 3, 188);

	// arrow
	set_win_tile_xy(wnd_x + 2, wnd_y + 2 + arrow_y, 16);

	IE_REG |= TIM_IFLAG;

	// text
	for (uint8_t idx = 0; idx < wnd_height; idx++) {
		txtwind_read_line(idx, &line);
		IE_REG &= ~TIM_IFLAG;
		set_win_tiles(wnd_x + 4, wnd_y + 2 + idx, line.len, 1, line.text);
		IE_REG |= TIM_IFLAG;
	}

	wait_vbl_done();
	gbc_init_set_palette();

	uint8_t keys;
	while (true) {
		wait_vbl_done();
		wait_vbl_done();
		keys = joypad();

		IE_REG &= ~TIM_IFLAG;

		if (keys & J_UP) {
			if (arrow_y > 0) {
				set_win_tile_xy(wnd_x + 2, wnd_y + 2 + arrow_y, 0);
				arrow_y--;
				set_win_tile_xy(wnd_x + 2, wnd_y + 2 + arrow_y, 16);
			}
		} else if (keys & J_DOWN) {
			if (arrow_y < (txtwind_lines - 1)) {
				set_win_tile_xy(wnd_x + 2, wnd_y + 2 + arrow_y, 0);
				arrow_y++;
				set_win_tile_xy(wnd_x + 2, wnd_y + 2 + arrow_y, 16);
			}
		} else if (keys & J_A) {
			result = arrow_y;
			break;
		} else if (keys & J_B) {
			break;
		}

		IE_REG |= TIM_IFLAG;

		wait_vbl_done();
		wait_vbl_done();
	}

	while (keys != 0) {
		wait_vbl_done();
		keys = joypad();
	}

	return result;
}

uint8_t txtwind_run(uint8_t render_mode) BANKED {
	uint8_t old_draw_offset_x = draw_offset_x;
	uint8_t old_draw_offset_y = draw_offset_y;

	uint8_t result;

	wait_vbl_done();
	text_reinit(render_mode);

	if (render_mode == RENDER_MODE_TXTWIND) {
		result = txtwind_run_txtwind();
	} else if (render_mode == RENDER_MODE_MENU) {
		result = txtwind_run_menu();
	}

	gbc_init_unset_palette();
	wait_vbl_done();
	game_update_sidebar_all();
	input_reset();
	text_reinit(RENDER_MODE_PLAYFIELD);
	draw_offset_x = old_draw_offset_x;
	draw_offset_y = old_draw_offset_y;
	text_update();
	return result;
}

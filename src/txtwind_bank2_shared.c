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

	ZOO_ENABLE_RAM;
	sram_write8(&ptr, line_ptr);
	sram_write8(&ptr, line_ptr >> 8);
	sram_write8(&ptr, line_bank);
	ZOO_DISABLE_RAM;

	txtwind_lines++;
}

bool txtwind_exec_line(uint16_t idx) {
	txtwind_line_t line;
	txtwind_read_line(idx, &line);

	if (line.type == TXTWIND_LINE_TYPE_HYPERLINK) {
		if (oop_send_target(line.text[line.len], false, line.text[line.len + 1], false)) {
			return true;
		}
	}

	return false;
}

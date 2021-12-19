#include "gamevars.h"
#include "sram_alloc.h"
#pragma bank 2

#include <gb/gb.h>
#include "config.h"
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

void txtwind_get_line(uint16_t idx, far_ptr_t *line_ptr) BANKED {
	sram_ptr_t ptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (idx * 3);

	ENABLE_RAM_MBC5;
	sram_read(&ptr, line_ptr, 3);
	DISABLE_RAM_MBC5;
}
#include <gb/gb.h>
#include <string.h>
#include "config.h"
#include "gamevars.h"
#include "sram_alloc.h"
#include "txtwind.h"

void txtwind_read_line(uint16_t idx, txtwind_line_t *line) {
	if (idx >= txtwind_lines) {
		line->type = TXTWIND_LINE_TYPE_REGULAR;
		line->len = 0;
		return;
	}

	sram_ptr_t ptr;
	far_ptr_t fptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (idx * 3);

	ENABLE_RAM_MBC5;
	sram_read(&ptr, &fptr, 3);
	DISABLE_RAM_MBC5;

	uint8_t prev_bank = _current_bank;
	SWITCH_ROM_MBC5(fptr.bank);
	const txtwind_line_t *tptr = (const txtwind_line_t*) fptr.ptr;
	memcpy(line, tptr, 3 + tptr->len);
	SWITCH_ROM_MBC5(prev_bank);
}

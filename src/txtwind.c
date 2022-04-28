#include <gb/gb.h>
#include <string.h>

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "sram_alloc.h"
#include "txtwind.h"

void txtwind_read_line(int16_t idx, txtwind_line_t *line) {
	if (idx < 0 || idx >= ((int16_t) txtwind_lines)) {
		line->type = TXTWIND_LINE_TYPE_REGULAR;
		line->len = 0;
		return;
	}

	sram_ptr_t ptr;
	far_ptr_t fptr;
	ptr.bank = 0;
	ptr.position = SRAM_TEXT_WINDOW_POS + (idx * 3);

	ZOO_ENABLE_RAM;
	sram_read(&ptr, &fptr, 3);
	ZOO_DISABLE_RAM;

	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(fptr.bank);
	const txtwind_line_t *tptr = (const txtwind_line_t*) fptr.ptr;
	memcpy(line, tptr, TXTWIND_LINE_HEADER_LEN + tptr->len);
	ZOO_SWITCH_ROM(prev_bank);
}

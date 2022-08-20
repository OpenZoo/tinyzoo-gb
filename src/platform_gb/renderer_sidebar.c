#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "renderer.h"
#include "renderer_sidebar.h"

AT(0xC040) uint8_t sidebar_tile_data[96];
extern uint8_t sidebar_tile_data_ly_switch;

extern uint8_t ly_bank_switch;
extern uint8_t ly_offset;

void sidebar_copy_data(uint16_t address, uint8_t len) {
	safe_vmemcpy((uint8_t*) address, sidebar_tile_data, len >> 1);
}

static uint8_t sb_offset;

static void sidebar_show_line(const char *line1) {
	if (line1 != NULL) {
		uint8_t slen = *(line1++);
		if (slen > 0) {
			memset(sidebar_tile_data + sb_offset, 0, 20);
			memcpy(sidebar_tile_data + sb_offset + (10 - (slen >> 1)), line1, slen);
			sb_offset += 32;
		}
	}
}

void sidebar_show_message(const char* line1, uint8_t bank1, const char* line2, uint8_t bank2, const char* line3, uint8_t bank3) {
	sb_offset = 0;

	// write text data
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(bank1);
	sidebar_show_line(line1);
	ZOO_SWITCH_ROM(bank2);
	sidebar_show_line(line2);
	ZOO_SWITCH_ROM(bank3);
	sidebar_show_line(line3);
	ZOO_SWITCH_ROM(prev_bank);

	sidebar_tile_data_ly_switch = ly_offset - (sb_offset >> 2);
#ifdef HACK_HIDE_STATUSBAR
	sidebar_copy_data(0x9C00 + (15 << 5) + (96 - sb_offset), sb_offset);
#else
	sidebar_copy_data(0x9C00 + (14 << 5) + (96 - sb_offset), sb_offset);
#endif
}

void sidebar_hide_message(void) {
	ly_bank_switch = ly_offset;
}

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gb/gb.h>
#include "renderer_sidebar.h"
#include "sram_debug.h"

__at(0xC040) uint8_t sidebar_tile_data[96];

extern uint8_t sidebar_tile_data_ly_switch;
extern uint8_t sidebar_tile_data_len;
extern uint16_t sidebar_tile_data_address;
extern volatile bool sidebar_tile_data_awaiting;

extern uint8_t ly_bank_switch;

void sidebar_copy_data(uint16_t address, uint8_t len) {
	sidebar_tile_data_address = address;
	sidebar_tile_data_len = len;
	sidebar_tile_data_awaiting = true;
}

static uint8_t sb_offset;

static void sidebar_show_line(const char *line1) {
	if (line1 != NULL) {
		uint8_t slen = strlen(line1);
		if (slen > 0) {
			memcpy(sidebar_tile_data + sb_offset + (10 - (slen >> 1)), line1, slen);
			sb_offset += 32;
		}
	}
}

void sidebar_show_message_nobank(const char* line1, uint8_t bank1, const char* line2, uint8_t bank2, const char* line3, uint8_t bank3) {
	while (sidebar_tile_data_awaiting) {}
	memset(sidebar_tile_data, 0, 84);
	sb_offset = 0;

	// write text data
	uint8_t prev_bank = _current_bank;
	SWITCH_ROM_MBC5(bank1);
	sidebar_show_line(line1);
	SWITCH_ROM_MBC5(bank2);
	sidebar_show_line(line2);
	SWITCH_ROM_MBC5(bank3);
	sidebar_show_line(line3);
	SWITCH_ROM_MBC5(prev_bank);

	sidebar_tile_data_ly_switch = 135 - (sb_offset >> 2);
	sidebar_copy_data(0x9C00 + (14 << 5) + (96 - sb_offset), sb_offset);
}

void sidebar_hide_message(void) {
	ly_bank_switch = 135;
}
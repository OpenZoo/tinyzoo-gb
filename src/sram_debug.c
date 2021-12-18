#include "gb/gb.h"
#pragma bank 2

#include <string.h>
#include "config.h"
#include "gamevars.h"

#ifdef SRAM_DEBUG
static uint16_t sram_debug_pos = 0;

void sram_debug8(uint8_t val) BANKED {
	ENABLE_RAM_MBC5;
	SWITCH_RAM_MBC5(SRAM_MAX_BANK - 1);
	*((uint8_t*) (0xA000 | sram_debug_pos)) = val;
	sram_debug_pos = (sram_debug_pos + 1) & 0x1FFF;
	DISABLE_RAM_MBC5;
}
#endif
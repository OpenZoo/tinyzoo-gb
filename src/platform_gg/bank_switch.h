#ifndef __BANK_SWITCH_H__
#define __BANK_SWITCH_H__

#include <stdint.h>
#include <gbdk/platform.h>

uint8_t zoo_get_ram_bank_count(void) BANKED;

#define ZOO_INIT_ROM {}

#define ZOO_SWITCH_ROM(bank) \
	{ \
		_current_bank = (bank); \
		MAP_FRAME1 = (bank); \
		__asm; \
			nop \
		__endasm; \
	}

#define ZOO_SWITCH_RAM(bank) \
	{ \
		SWITCH_RAM(bank); \
		__asm; \
			nop \
		__endasm; \
	}

#define ZOO_ENABLE_RAM ENABLE_RAM
#define ZOO_DISABLE_RAM DISABLE_RAM

#endif

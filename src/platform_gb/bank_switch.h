#ifndef __BANK_SWITCH_H__
#define __BANK_SWITCH_H__

#include <stdint.h>
#include <gbdk/platform.h>

uint8_t zoo_get_ram_bank_count(void) BANKED;

#ifdef __TPP1__

#define ZOO_INIT_ROM \
	{ \
		_current_bank = 1; \
	}

#define ZOO_SWITCH_ROM(bank) \
	{ \
		_current_bank = (bank); \
		*(volatile uint8_t *)0x0000 = (bank); \
	}

#define ZOO_SWITCH_RAM(bank) \
	{ \
		*(volatile uint8_t *)0x0002 = (bank); \
	}

#define ZOO_ENABLE_RAM \
	{ \
		*(volatile uint8_t *)0x0003 = 0x03; \
	}

#define ZOO_DISABLE_RAM \
	{ \
		*(volatile uint8_t *)0x0003 = 0x00; \
	}

#else /* __MBC5__ */

#define ZOO_INIT_ROM {}

#define ZOO_SWITCH_ROM(bank) \
	{ \
		_current_bank = (bank); \
		*(volatile uint8_t *)0x2000 = (bank); \
	}

#define ZOO_SWITCH_RAM(bank) \
	{ \
		*(volatile uint8_t *)0x4000 = (bank); \
	}

#define ZOO_ENABLE_RAM ENABLE_RAM_MBC5
#define ZOO_DISABLE_RAM DISABLE_RAM_MBC5

#endif

#endif

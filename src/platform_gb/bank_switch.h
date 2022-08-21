/**
 * TinyZoo/GB utility code
 *
 * Copyright (c) 2021, 2022 Adrian Siekierka
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

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

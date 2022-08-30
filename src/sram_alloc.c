#pragma bank 2

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define SRAM_ALLOC_INTERNAL

#include <string.h>

#include "bank_switch.h"
#include "config.h"
#include "gamevars.h"
#include "sram_alloc.h"

typedef struct {
	uint8_t flags;
	uint16_t size;
} sram_entry_t;

#define SRAM_HEADER_FLAG_WORLD 0x01
#define SRAM_FLAG_USED 0x01

static void sram_inc_ptr(sram_ptr_t *ptr) {
	if ((++(ptr->position)) == SRAM_BANK_SIZE) {
		ptr->position = 0;
		ptr->bank++;
	}
}

void sram_add_ptr(sram_ptr_t *ptr, uint16_t val) {
	while (val >= SRAM_BANK_SIZE) {
		ptr->bank++;
		val -= SRAM_BANK_SIZE;
	}
	ptr->position += val;
	while (ptr->position >= SRAM_BANK_SIZE) {
		ptr->position -= SRAM_BANK_SIZE;
		ptr->bank++;
	}
}

void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val) {
	while (val >= SRAM_BANK_SIZE) {
		ptr->bank--;
		val -= SRAM_BANK_SIZE;
	}
	ptr->position -= val;
	while (ptr->position < 0) {
		ptr->position += SRAM_BANK_SIZE;
		ptr->bank--;
	}
}

#ifdef SM83
uint8_t sram_read8(sram_ptr_t *ptr) {
	// de = ptr
__asm
	ld	h, d
	ld	l, e
	ld      a, (hl+)
	ld	c, a ; c = bank
#ifdef __TPP1__
        ld      (0x0002), a
#else
        ld      (0x4000), a
#endif
	ld	a, (hl+)
	ld	e, a
	ld	a, (hl)
	or	a, #0xA0
	ld	d, a
	ld	a, b
	ld	a, (de)
	inc	de
	ld	(0xFFA7), a
	ld	a, d
	ld	b, a
	and	a, #0x1F
	ld	(hl-), a
	ld	a, e
	ld	(hl-), a
	bit	5, b ; position overflow?
	jr	nz, SramRead8Finish
	inc	c
	ld	(hl), c ; increment bank
SramRead8Finish:
	ld	a, (0xFFA7)
__endasm;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	// de = ptr
	// a = value
__asm
	ld	h, d
	ld	l, e
	ld	b, a ; b - value
	ld      a, (hl+)
	ld	c, a ; c = bank
#ifdef __TPP1__
        ld      (0x0002), a
#else
        ld      (0x4000), a
#endif
	ld	a, (hl+)
	ld	e, a
	ld	a, (hl)
	or	a, #0xA0
	ld	d, a
	ld	a, b
	ld	(de), a ; set value
	inc	de
	ld	a, d
	ld	b, a
	and	a, #0x1F
	ld	(hl-), a
	ld	a, e
	ld	(hl-), a
	bit	5, b ; position overflow?
	ret	nz
	inc	c
	ld	(hl), c ; increment bank
__endasm;
}
#else
uint8_t sram_read8(sram_ptr_t *ptr) {
	ZOO_SWITCH_RAM(ptr->bank);
	uint8_t value = ((uint8_t*) SRAM_ADDRESS)[ptr->position];
	sram_inc_ptr(ptr);
	return value;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	ZOO_SWITCH_RAM(ptr->bank);
	((uint8_t*) SRAM_ADDRESS)[ptr->position] = value;
	sram_inc_ptr(ptr);
}
#endif

void sram_read(sram_ptr_t *ptr, uint8_t *data, uint16_t len) {
#if 1
	while (len > 0) {
		ZOO_SWITCH_RAM(ptr->bank);

		uint16_t len_to_read = SRAM_BANK_SIZE - ptr->position;
		if (len_to_read > len) {
			len_to_read = len;
			memcpy(data, ((uint8_t*) (SRAM_ADDRESS + ptr->position)), len_to_read);
			sram_add_ptr(ptr, len_to_read);
		} else {
			memcpy(data, ((uint8_t*) (SRAM_ADDRESS + ptr->position)), len_to_read);
			ptr->bank++;
			ptr->position = 0;
		}
		data += len_to_read;
		len -= len_to_read;
	}
#else
	while ((len--) > 0) {
		*(data++) = sram_read8(ptr);
	}
#endif
}

void sram_write(sram_ptr_t *ptr, const uint8_t *data, uint16_t len) {
#if 1
	while (len > 0) {
		ZOO_SWITCH_RAM(ptr->bank);

		uint16_t len_to_read = SRAM_BANK_SIZE - ptr->position;
		if (len_to_read > len) {
			len_to_read = len;
			memcpy(((uint8_t*) (SRAM_ADDRESS + ptr->position)), data, len_to_read);
			sram_add_ptr(ptr, len_to_read);
		} else {
			memcpy(((uint8_t*) (SRAM_ADDRESS + ptr->position)), data, len_to_read);
			ptr->bank++;
			ptr->position = 0;
		}
		data += len_to_read;
		len -= len_to_read;
	}
#else
	while ((len--) > 0) {
		sram_write8(ptr, *(data++));
	}
#endif
}

#ifdef DEBUG_SRAM_WRITES
uint8_t sram_read8_debug(sram_ptr_t *ptr) {
	EMU_printf("reading 1 from %02X:%04X",
		(uint16_t) ptr->bank, (uint16_t) ptr->position);
	return sram_read8(ptr);
}

void sram_write8_debug(sram_ptr_t *ptr, uint8_t value) {
	sram_ptr_t tptr, dptr;
	uint8_t tvalue;
	tptr = *ptr;
	dptr = *ptr;
	EMU_printf("writing 1 to %02X:%04X",
		(uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_write8(ptr, value);
	if ((tvalue = sram_read8(&tptr)) != value) {
		EMU_printf("data mismatch %02X:%04X - exp %d, act %d",
			(uint16_t) dptr.bank, (uint16_t) dptr.position,
			(uint16_t) value, (uint16_t) tvalue);
	}
}

void sram_read_debug(sram_ptr_t *ptr, uint8_t *data, uint16_t len) {
	EMU_printf("reading %d from %02X:%04X",
		len, (uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_read(ptr, data, len);
}

void sram_write_debug(sram_ptr_t *ptr, const uint8_t *data, uint16_t len) {
	sram_ptr_t tptr, dptr;
	uint8_t tvalue;
	tptr = *ptr;
	EMU_printf("writing %d to %02X:%04X",
		len, (uint16_t) ptr->bank, (uint16_t) ptr->position);
	sram_write(ptr, data, len);
	for (uint16_t i = 0; i < len; i++) {
		dptr = tptr;
		if ((tvalue = sram_read8(&tptr)) != data[i]) {
			EMU_printf("data mismatch %02X:%04X - exp %d, act %d",
				(uint16_t) dptr.bank, (uint16_t) dptr.position,
				(uint16_t) data[i], (uint16_t) tvalue);
		}
	}
}
#endif

bool sram_alloc(uint16_t len, sram_ptr_t *ptr) {
	uint16_t offset, nlen;
	sram_entry_t entry;

	offset = 0;
	ptr->bank = 0;
	ptr->position = sizeof(sram_header_t);

	uint8_t max_bank = zoo_get_ram_bank_count();
	while (ptr->bank < max_bank) {
		sram_read(ptr, (uint8_t*) &entry, sizeof(sram_entry_t));
		if ((entry.flags & SRAM_FLAG_USED) != 0) {
			offset = 0;
		} else {
			nlen = entry.size + offset;
			if (nlen >= len) {
				if ((nlen - len) <= sizeof(sram_entry_t)) {
					len = nlen;
				}
				// rewind to first entry location
				sram_sub_ptr(ptr, sizeof(sram_entry_t) + offset);
				// allocate entry of size len
				entry.flags = SRAM_FLAG_USED;
				entry.size = len;
				sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
				if (nlen > len) {
					// add new free entry
					sram_add_ptr(ptr, len);
					entry.flags = 0;
					entry.size = nlen - len - sizeof(sram_entry_t);
					sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
					sram_sub_ptr(ptr, len + sizeof(sram_entry_t));
				}
				return true;
			} else {
				offset += sizeof(sram_entry_t) + entry.size;
			}
		}
		sram_add_ptr(ptr, entry.size);
	}

	// TODO: block shifting? is it even doable?
	return false;
}

void sram_free(sram_ptr_t *ptr) {
	sram_entry_t entry;
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
	sram_read(ptr, (uint8_t*) &entry, sizeof(sram_entry_t));
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
	entry.flags &= ~SRAM_FLAG_USED;
	sram_write(ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
}

static const uint8_t sram_expected_magic[4] = {'T', 'n', 'Z', 0x01};

void sram_toggle_write(void) {
	ZOO_SWITCH_RAM(0);
	((uint8_t*) SRAM_ADDRESS)[0] ^= 0x20;
}

void sram_init(bool force) BANKED {
	uint8_t magic[4];
	sram_ptr_t ptr;
	sram_entry_t entry;

	ptr.position = 0;
	ptr.bank = 0;

	ZOO_ENABLE_RAM;

#ifndef RESET_SAVE_ON_START
	sram_read(&ptr, magic, 4);
	if (magic[0] != sram_expected_magic[0]
		|| magic[1] != sram_expected_magic[1]
		|| magic[2] != sram_expected_magic[2]
		|| magic[3] != sram_expected_magic[3]
		|| force)
#endif
	{
		ptr.position = 0;
		sram_write(&ptr, sram_expected_magic, 4);
		// write empty flags
		sram_write8(&ptr, 0);
		// write empty world id
		sram_write8(&ptr, 0xFF);

		// init linked list
		ptr.position = sizeof(sram_header_t);

		entry.flags = 0;
		entry.size = (16384 - sizeof(sram_entry_t) - sizeof(sram_header_t));
		sram_write(&ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
		sram_add_ptr(&ptr, entry.size);

		entry.size += sizeof(sram_header_t);

		uint8_t max_bank = zoo_get_ram_bank_count();
		while (ptr.bank < max_bank) {
			sram_write(&ptr, (const uint8_t*) &entry, sizeof(sram_entry_t));
			sram_add_ptr(&ptr, entry.size);
		}
	}

	ZOO_DISABLE_RAM;
}

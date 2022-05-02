#pragma bank 2

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
	if ((++(ptr->position)) == 0x2000) {
		ptr->position = 0;
		ptr->bank++;
	}
}

void sram_add_ptr(sram_ptr_t *ptr, uint16_t val) {
	while (val >= 0x2000) {
		ptr->bank++;
		val -= 0x2000;
	}
	ptr->position += val;
	if (ptr->position >= 0x2000) {
		ptr->position -= 0x2000;
		ptr->bank++;
	}
}

void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val) {
	while (val >= 0x2000) {
		ptr->bank--;
		val -= 0x2000;
	}
	ptr->position -= val;
	if (ptr->position >= 0xE000) {
		ptr->position += 0x2000;
		ptr->bank--;
	}
}

#ifdef SM83
uint8_t sram_read8(sram_ptr_t *ptr) {
	// de = ptr
__asm
        ld      a, (de)
	ld	c, a ; c = bank
#ifdef __TPP1__
        ld      (0x0002), a
#else
        ld      (0x4000), a
#endif
	inc	de
	ld	a, (de)
	ld	l, a
	inc	de
	ld	a, (de)
	or	a, #0xA0
	ld	h, a
	ld	a, b
	ld	a, (hl+)
	ld	(0xFFA0), a
	ld	a, h
	ld	b, a
	and	a, #0x1F
	ld	(de), a
	dec	de
	ld	a, l
	ld	(de), a
	bit	5, b ; position overflow?
	jr	nz, SramRead8Finish
	dec	de
	ld	a, c
	inc	a
	ld	(de), a ; increment bank
SramRead8Finish:
	ld	a, (0xFFA0)
__endasm;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	// de = ptr
	// a = value
__asm
	ld	b, a ; b - value
        ld      a, (de)
	ld	c, a ; c = bank
#ifdef __TPP1__
        ld      (0x0002), a
#else
        ld      (0x4000), a
#endif
	inc	de
	ld	a, (de)
	ld	l, a
	inc	de
	ld	a, (de)
	or	a, #0xA0
	ld	h, a
	ld	a, b
	ld	(hl+), a ; set value
	ld	a, h
	ld	b, a
	and	a, #0x1F
	ld	(de), a
	dec	de
	ld	a, l
	ld	(de), a
	bit	5, b ; position overflow?
	ret	nz
	dec	de
	ld	a, c
	inc	a
	ld	(de), a ; increment bank
__endasm;
}
#else
uint8_t sram_read8(sram_ptr_t *ptr) {
	ZOO_SWITCH_RAM(ptr->bank);
	uint8_t value = ((uint8_t*) 0xA000)[ptr->position];
	sram_inc_ptr(ptr);
	return value;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	ZOO_SWITCH_RAM(ptr->bank);
	((uint8_t*) 0xA000)[ptr->position] = value;
	sram_inc_ptr(ptr);
}
#endif

void sram_read(sram_ptr_t *ptr, uint8_t *data, uint16_t len) {
#if 1
	while (len > 0) {
		uint16_t len_to_read = 0x2000 - ptr->position;
		if (len_to_read > len) {
			len_to_read = len;
		}
		ZOO_SWITCH_RAM(ptr->bank);
		memcpy(data, ((uint8_t*) 0xA000) + ptr->position, len_to_read);
		sram_add_ptr(ptr, len_to_read);
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
		uint16_t len_to_read = 0x2000 - ptr->position;
		if (len_to_read > len) {
			len_to_read = len;
		}
		ZOO_SWITCH_RAM(ptr->bank);
		memcpy(((uint8_t*) 0xA000) + ptr->position, data, len_to_read);
		sram_add_ptr(ptr, len_to_read);
		data += len_to_read;
		len -= len_to_read;
	}
#else
	while ((len--) > 0) {
		sram_write8(ptr, *(data++));
	}
#endif
}

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

static const uint8_t sram_expected_magic[4] = {'G', 'b', 'Z', 0x01};

void sram_toggle_write(void) {
	ZOO_SWITCH_RAM(0);
	((uint8_t*) 0xA000)[0] ^= 0x20;
}

void sram_init(bool force) BANKED {
	uint8_t magic[4];
	sram_ptr_t ptr;
	sram_entry_t entry;

	ptr.position = 0;
	ptr.bank = 0;

	ZOO_ENABLE_RAM;

	sram_read(&ptr, magic, 4);
	if (magic[0] != sram_expected_magic[0]
		|| magic[1] != sram_expected_magic[1]
		|| magic[2] != sram_expected_magic[2]
		|| magic[3] != sram_expected_magic[3]
		|| force)
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

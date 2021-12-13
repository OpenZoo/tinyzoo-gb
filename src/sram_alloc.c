#include "gb/gb.h"
#pragma bank 2

#include <string.h>
#include "config.h"
#include "sram_alloc.h"

typedef struct {
	uint8_t flags;
	uint16_t size;
} sram_entry_t;

typedef struct {
	uint8_t magic[4];
	sram_ptr_t current_world_data;
} sram_header_t;

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

uint8_t sram_read8(sram_ptr_t *ptr) {
	SWITCH_RAM_MBC5(ptr->bank);
	uint8_t value = ((uint8_t*) 0xA000)[ptr->position];
	sram_inc_ptr(ptr);
	return value;
}

void sram_write8(sram_ptr_t *ptr, uint8_t value) {
	SWITCH_RAM_MBC5(ptr->bank);
	((uint8_t*) 0xA000)[ptr->position] = value;
	sram_inc_ptr(ptr);
}

void sram_read(sram_ptr_t *ptr, void *data, uint16_t len) {
	uint16_t i = 0;
	while (i < len) {
		((uint8_t*) data)[i] = sram_read8(ptr);
		i++;
	}
}

void sram_write(sram_ptr_t *ptr, const void *data, uint16_t len) {
	uint16_t i = 0;
	while (i < len) {
		sram_write8(ptr, ((const uint8_t*) data)[i]);
		i++;
	}
}

bool sram_alloc(uint16_t len, sram_ptr_t *ptr) {
	uint16_t offset, nlen;
	sram_entry_t entry;

	offset = 0;
	ptr->bank = 0;
	ptr->position = sizeof(sram_header_t);

	while (ptr->bank < SRAM_MAX_BANK) {
		sram_read(ptr, &entry, sizeof(sram_entry_t));
		if ((entry.flags & SRAM_FLAG_USED) != 0) {
			offset = 0;
		} else {
			if ((entry.size + offset) >= len) {
				nlen = entry.size + offset;
				// rewind to first entry location
				sram_sub_ptr(ptr, sizeof(sram_entry_t) + offset);
				// allocate entry of size len
				entry.flags = SRAM_FLAG_USED;
				entry.size = len;
				sram_write(ptr, &entry, sizeof(sram_entry_t));
				if (nlen > len) {
					// add new free entry
					sram_add_ptr(ptr, len);
					entry.flags = 0;
					entry.size = nlen - len;
					sram_write(ptr, &entry, sizeof(sram_entry_t));
					sram_sub_ptr(ptr, len + sizeof(sram_entry_t));
				}
				return true;
			} else {
				if (offset > 0) offset += sizeof(sram_entry_t);
				offset += entry.size;
			}
		}
		sram_add_ptr(ptr, entry.size);
	}

	// TODO: block shifting? is it even doable?
	return false;
}

void sram_free(sram_ptr_t *ptr) {
	sram_entry_t entry;
	sram_read(ptr, &entry, sizeof(sram_entry_t));
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
	entry.flags &= ~SRAM_FLAG_USED;
	sram_write(ptr, &entry, sizeof(sram_entry_t));
	sram_sub_ptr(ptr, sizeof(sram_entry_t));
}

static const uint8_t sram_expected_magic[4] = {'G', 'b', 'Z', 0x01};

void sram_init(bool force) BANKED {
	sram_header_t header;
	sram_ptr_t ptr;
	sram_entry_t entry;

	ptr.position = 0;
	ptr.bank = 0;

	ENABLE_RAM_MBC5;

	sram_read(&ptr, &header, sizeof(sram_header_t));
	if (header.magic[0] != sram_expected_magic[0]
		|| header.magic[1] != sram_expected_magic[1]
		|| header.magic[2] != sram_expected_magic[2]
		|| header.magic[3] != sram_expected_magic[3]
		|| force)
	{
		ptr.position = 0;
		sram_write(&ptr, &sram_expected_magic, sizeof(sram_header_t));

		entry.flags = 0;
		entry.size = (16384 - sizeof(sram_entry_t) - sizeof(sram_header_t));
		sram_write(&ptr, &entry, sizeof(sram_entry_t));
		sram_add_ptr(&ptr, entry.size);

		entry.size += sizeof(sram_header_t);

		while (ptr.bank < SRAM_MAX_BANK) {
			sram_write(&ptr, &entry, sizeof(sram_entry_t));
			sram_add_ptr(&ptr, entry.size);
		}
	}

	DISABLE_RAM_MBC5;
}

#ifndef __SRAM_ALLOC_H__
#define __SRAM_ALLOC_H__

#include <stdint.h>
#include <stdbool.h>
#include <gb/gb.h>

typedef struct {
	uint8_t bank;
	uint16_t position;
} sram_ptr_t;

// Pre-setup required: ENABLE_RAM, switch bank
void sram_add_ptr(sram_ptr_t *ptr, uint16_t val);
void sram_sub_ptr(sram_ptr_t *ptr, uint16_t val);

uint8_t sram_read8(sram_ptr_t *ptr);
void sram_write8(sram_ptr_t *ptr, uint8_t value);

void sram_read(sram_ptr_t *ptr, void *data, uint16_t len);
void sram_write(sram_ptr_t *ptr, const void *data, uint16_t len);

bool sram_alloc(uint16_t len, sram_ptr_t *ptr);
void sram_free(sram_ptr_t *ptr);

// No pre-setup required.
void sram_init(bool force) BANKED;

#endif
#ifndef __SRAM_DEBUG_H__
#define __SRAM_DEBUG_H__

#include <stdint.h>
#include <stdbool.h>
#include <gb/gb.h>
#include "config.h"

#ifdef SRAM_DEBUG
void sram_debug8(uint8_t val) BANKED;
#else
#define sram_debug8(val)
#endif

#endif
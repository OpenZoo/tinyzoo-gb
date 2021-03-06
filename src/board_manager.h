#ifndef __BOARD_MANAGER_H__
#define __BOARD_MANAGER_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>
#include "sram_alloc.h"

// on bank 0 - do not use
void load_world_rom(uint8_t offset) NONBANKED;
void load_board_rom(uint8_t offset) NONBANKED;
void load_board_data_rom(uint8_t bank, const uint8_t *data) NONBANKED;

// on bank 2 - BANKED
void load_world(uint8_t offset, bool new_game) BANKED;
void load_board(uint8_t offset) BANKED;
bool save_world(uint8_t offset) BANKED;
bool save_board(uint8_t offset) BANKED;
bool clear_saved_board(uint8_t offset) BANKED;

#endif /* __BOARD_MANAGER_H__ */

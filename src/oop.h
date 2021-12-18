#ifndef __OOP_H__
#define __OOP_H__

#include <stdint.h>
#include <stdbool.h>
#include <gb/gb.h>
#include "config.h"
#include "gamevars.h"

// defines
#define OOP_DATA_POS_END 0xFFFF

#define OOP_LABEL_BOMBED 250
#define OOP_LABEL_TOUCH 251
#define OOP_LABEL_THUD 252
#define OOP_LABEL_ENERGIZE 253
#define OOP_LABEL_SHOT 254

#define OOP_TARGET_SELF 252
#define OOP_TARGET_OTHERS 253
#define OOP_TARGET_ALL 254

// oop_flags.c (bank 0)
#define FLAG_ID_NONE 255

uint8_t world_get_flag_pos(uint8_t flag_id);
void world_set_flag(uint8_t flag_id);
void world_clear_flag(uint8_t flag_id);

// oop_bank2.c (bank 2)
bool find_tile_on_board(uint8_t *x, uint8_t *y, uint8_t element, uint8_t color) BANKED;
void oop_place_tile(uint8_t x, uint8_t y, uint8_t element, uint8_t color) BANKED;

// oop.c (bank 0)
extern const char *oop_object_name;
extern const char *oop_scroll_name;
void oop_send(uint8_t stat_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
void oop_send_target(uint8_t target_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
bool oop_execute(uint8_t stat_id, const char *name);

#endif
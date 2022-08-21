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

#ifndef __OOP_H__
#define __OOP_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>
#include "config.h"
#include "gamevars.h"

// defines
#define OOP_DATA_POS_END 0xFFFF

#define OOP_LABEL_BOMBED 249
#define OOP_LABEL_TOUCH 250
#define OOP_LABEL_THUD 251
#define OOP_LABEL_ENERGIZE 252
#define OOP_LABEL_SHOT 253
#define OOP_LABEL_RESTART 254
#define OOP_LABEL_VOID 255

#define OOP_TARGET_EMPTY 251
#define OOP_TARGET_SELF 252
#define OOP_TARGET_OTHERS 253
#define OOP_TARGET_ALL 254
#define OOP_TARGET_VOID 255

// oop_flags.c (bank 0)
#define FLAG_ID_NONE 255

uint8_t world_get_flag_pos(uint8_t flag_id);
void world_set_flag(uint8_t flag_id);
void world_clear_flag(uint8_t flag_id);

// oop_bank2.c (bank 2)
bool find_tile_on_board(uint8_t *x, uint8_t *y, uint8_t element, uint8_t color) BANKED;
void oop_place_tile(uint8_t x, uint8_t y, uint8_t element, uint8_t color) BANKED;
uint16_t oop_dataofs_clone(uint16_t loc) BANKED;
void oop_dataofs_free_if_unused(uint16_t loc, uint8_t except_id) BANKED;

// oop.c (bank 0)
extern const char oop_object_name[];
extern const char oop_scroll_name[];
bool oop_send(uint8_t stat_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
bool oop_send_target(uint8_t target_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock);
bool oop_execute(uint8_t stat_id, const char *name);

#endif

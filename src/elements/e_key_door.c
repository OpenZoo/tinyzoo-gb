#pragma bank 1

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

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../../res/message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

const char *msg_key_pickup_get(uint8_t x) BANKED;
const char *msg_door_open_get(uint8_t x) BANKED;

static void show_key_pickup_no(uint8_t key) {
	display_message(200, NULL, msg_key_pickup_no, msg_key_pickup_get(key));
	sound_queue(2, sound_key_failure);
}

static void show_key_pickup_yes(uint8_t key) {
	display_message(200, NULL, msg_key_pickup_yes, msg_key_pickup_get(key));
	sound_queue(2, sound_key_success);
}

static void show_door_open_no(uint8_t key) {
	display_message(200, NULL, msg_door_open_get(key), msg_door_open_no);
	sound_queue(3, sound_door_failure);
}

static void show_door_open_yes(uint8_t key) {
	display_message(200, NULL, msg_door_open_get(key), msg_door_open_yes);
	sound_queue(3, sound_door_success);
}

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color & 7);
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) != 0) {
			show_key_pickup_no(0);
		} else {
			zoo_world_info.gems |= 0x100;
			tile->element = E_EMPTY;
			game_update_sidebar_gems_time();

			show_key_pickup_yes(0);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (zoo_world_info.keys & key_shift) {
			show_key_pickup_no(key);
		} else {
			zoo_world_info.keys |= key_shift;
			tile->element = E_EMPTY;
			game_update_sidebar_keys();

			show_key_pickup_yes(key);
		}
	}
}

void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color >> 4) & 7;
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) == 0) {
			show_door_open_no(0);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.gems &= 0x00FF;
			game_update_sidebar_gems_time();

			show_door_open_yes(0);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (!(zoo_world_info.keys & key_shift)) {
			show_door_open_no(key);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.keys &= ~key_shift;
			game_update_sidebar_keys();

			show_door_open_yes(key);
		}
	}
}

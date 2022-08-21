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

#include <gbdk/platform.h>
#include "input.h"

//#define JOY_REPEAT_DELAY 15
#define JOY_REPEAT_DELAY 11
#define JOY_REPEAT_DELAY_NEXT 2

uint8_t vbl_ticks;

static uint8_t input_keys = 0;
static uint8_t input_keys_repressed = 0;
static uint8_t input_keys_released = 0;
uint8_t input_held;
static uint8_t input_vbls_next[4];
static uint8_t input_last = 0;

int8_t input_delta_x, input_delta_y;

void input_update_vbl(void) {
	uint8_t keys = joypad();
	input_keys |= keys;
	input_keys_repressed |= (keys & input_keys_released);
	input_keys_released |= (input_held & (~keys));
}

void input_reset(void) {
	CRITICAL {
		input_keys = 0;
		input_keys_repressed = 0;
		input_keys_released = 0;
		input_update_vbl();
	}
}

void input_update(void) {
	uint8_t keys_pressed;
	uint8_t keys_repressed;
	uint8_t keys_released;

	CRITICAL {
		keys_pressed = input_keys;
		keys_repressed = input_keys_repressed;
		keys_released = input_keys_released;
	}

	input_delta_x = 0;
	input_delta_y = 0;

	input_held = (input_held & 0x0F) | (keys_pressed & 0xF0);
	if (keys_pressed & 0x0F) for (uint8_t i = 0; i < 4; i++) {
		uint8_t input_id = (input_last + 1) & 3;
		uint8_t input_mask = 1 << input_id;
		input_last = input_id;

		if (keys_pressed & input_mask) {
			if (keys_repressed & input_mask) {
				goto KeyRepressed;
			} else if (input_held & input_mask) {
				if (keys_released & input_mask) {
					input_held &= ~input_mask;
				}
				if (((uint8_t) (input_vbls_next[input_id] - vbl_ticks)) < 0x80) continue;
				if (!(keys_released & input_mask)) {
					input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY_NEXT;
				}
			} else {
				if (!(keys_released & input_mask)) {
KeyRepressed:
					input_held |= input_mask;
					input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY;
				}
			}
			if (input_mask == J_UP) {
				input_delta_y = -1;
			} else if (input_mask == J_DOWN) {
				input_delta_y = 1;
			} else if (input_mask == J_LEFT) {
				input_delta_x = -1;
			} else if (input_mask == J_RIGHT) {
				input_delta_x = 1;
			}
			break;
		}
	}

	input_reset();
}

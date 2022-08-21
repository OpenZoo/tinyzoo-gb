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

#include <string.h>
#include <gbdk/platform.h>
#include <gbdk/emu_debug.h>
#include "config.h"
#include "renderer.h"
#include "timer.h"

uint8_t sound_buffer[MAX_SOUND_BUFFER_SIZE];
volatile uint8_t sound_buffer_pos;
volatile uint8_t sound_buffer_len;
volatile uint8_t sound_duration_counter;
bool sound_enabled;
bool sound_block_queueing;
bool sound_is_playing;
int8_t sound_current_priority;

void sound_queue_nobank(int8_t priority, const uint8_t *data) {
	if (!sound_block_queueing) {
		if (!sound_is_playing || (((priority >= sound_current_priority) && (sound_current_priority != -1)) || (priority == -1))) {
			uint8_t data_len = data[0];
#ifdef DEBUG_SOUND_QUEUE
			EMU_printf("queueing sound: %d prio %d addr %02X:%04X",
				(uint16_t) data_len, (int16_t) priority, (uint16_t) _current_bank, (uint16_t) data);
#endif
			if (priority >= 0 || !sound_is_playing) {
				sound_current_priority = priority;
				text_sync_hblank_safe();
				CRITICAL {
					sound_duration_counter = 1;
					sound_buffer_len = data_len;
					sound_buffer_pos = 0;
					sound_buffer[0] = data[1];
					sound_buffer[1] = data[2];
				};
				if (data_len > 2) {
					memcpy(sound_buffer + 2, data + 3, data_len - 2);
				}
			} else {
				uint8_t pos = sound_buffer_pos;
				memmove(sound_buffer, sound_buffer + pos, sound_buffer_len - pos);
				text_sync_hblank_safe();
				CRITICAL {
					sound_buffer_len -= pos;
					sound_buffer_pos = 0;
				};
				if ((sound_buffer_len + data_len) <= MAX_SOUND_BUFFER_SIZE) {
					memcpy(sound_buffer + sound_buffer_len, data + 1, data_len);
					sound_buffer_len += data_len;
				}
			}
			sound_is_playing = true;
		}
	}
}

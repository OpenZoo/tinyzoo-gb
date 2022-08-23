#pragma bank 2

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
#include "config.h"
#include "timer.h"

typedef struct {
	uint8_t nr31, nr32;
	uint16_t nr33;
} gb_drum_data_t;

#define GB_DRUM_FREQ(hz) ((int) (2048 - (65536 / (hz))))

static const gb_drum_data_t sound_drums_data[] = {
	{255, 0x20, GB_DRUM_FREQ(3200) | 0xC000}, /* 0 */
	{252, 0x20, GB_DRUM_FREQ(1700) | 0xC000}, /* 1 */
	{252, 0x20, GB_DRUM_FREQ(3200) | 0xC000}, /* 2 */
	{243, 0x20, GB_DRUM_FREQ(1700) | 0xC000}, /* 3 */
	{252, 0x20, GB_DRUM_FREQ(3000) | 0xC000}, /* 4 */
	{252, 0x20, GB_DRUM_FREQ(1600) | 0xC000}, /* 5 */
	{252, 0x20, GB_DRUM_FREQ(1466) | 0xC000}, /* 6 */
	{252, 0x20, GB_DRUM_FREQ(616) | 0xC000}, /* 7 */
	{252, 0x20, GB_DRUM_FREQ(1200) | 0xC000}, /* 8 */
	{252, 0x20, GB_DRUM_FREQ(440) | 0xC000}  /* 9 */
};

extern uint8_t sound_buffer[MAX_SOUND_BUFFER_SIZE];
extern volatile uint8_t sound_buffer_pos;
extern volatile uint8_t sound_buffer_len;
extern volatile uint8_t sound_duration_counter;
extern bool sound_enabled;
extern bool sound_block_queueing;
extern bool sound_is_playing;
extern int8_t sound_current_priority;

void sound_update(void) {
__asm
	push bc

	; sound playback routine
	ld a, (_sound_enabled)
	or a, a
	jr z, .timer_handler_no_sound
	ld a, (_sound_is_playing)
	or a, a
	jr z, .timer_handler_sound_end

	ld a, (_sound_duration_counter)
	dec a
	ld (_sound_duration_counter), a
	; _sound_duration_counter == 0
	jr z, .timer_handler_next_note
	; _sound_duration_counter < 0
	bit 7, a
	jr z, .timer_handler_sound_end
	jr .timer_handler_next_note

.timer_handler_no_sound:
	xor a, a
	ld (_sound_is_playing), a
	ldh (_NR21_REG + 0), a
	ldh (_NR30_REG + 0), a
	ldh (_NR52_REG + 0), a

.timer_handler_sound_end:
	pop		bc
	ret

.timer_handler_next_note:
	ld a, (_sound_buffer_len)
	ld c, a
	ld a, (_sound_buffer_pos)
	cp a, c
	jr nc, .timer_handler_no_sound

	; init sound
	ld a, #0x80
	ldh (_NR52_REG + 0), a
	ld a, #0x66
	ldh (_NR51_REG + 0), a
	xor a, a
	ld b, a ; for sound_buffer_pos
	dec a
	; ld a, #0xFF
	ldh (_NR50_REG + 0), a

	ld a, (_sound_buffer_pos)
	ld c, a
	ld hl, #(_sound_buffer)
	add hl, bc ; hl = pointer to next entry

	ld a, (hl+) ; a = note
	push hl

.timer_handler_check_note:
	sub a, #16
	jr c, .timer_handler_check_drum
	cp a, #96
	jr nc, .timer_handler_check_drum
	add a, a
	ld c, a
	; ld b, #0x00 - still set from above
	ld hl, #(_sound_freqs)
	add hl, bc
	ld a, #0xF0
	ldh (_NR22_REG + 0), a
	ld a, (hl+)
	ldh (_NR23_REG + 0), a
	ld a, (hl)
	bit 7, a
	jr z, .timer_handler_checks_done ; dummy note
	ldh (_NR24_REG + 0), a
	ld a, #0x80
	ldh (_NR21_REG + 0), a
	ld a, #0x22
	; ldh (_NR51_REG + 0), a
	jr .timer_handler_checks_done

.timer_handler_check_drum:
	sub a, #224
	jr c, .timer_handler_checks_done
	cp a, #10
	jr nc, .timer_handler_checks_done
	add a, a
	add a, a
	ld c, a
	; ld b, #0x00 - still set from above
	ld hl, #(_sound_drums_data)
	add hl, bc
	ld a, (hl+)
	ldh (_NR31_REG + 0), a
	ld a, (hl+)
	ldh (_NR32_REG + 0), a
	ld a, (hl+)
	ldh (_NR33_REG + 0), a
	ld a, (hl)
	bit 7, a
	jr z, .timer_handler_checks_done ; dummy drum
	ldh (_NR34_REG + 0), a
	ld a, #0x80
	ldh (_NR30_REG + 0), a
	ld a, #0x44
	; ldh (_NR51_REG + 0), a

.timer_handler_checks_done:
	pop hl

	ld a, (hl) ; a = duration counter
	ld (_sound_duration_counter), a

	; update sound_buffer_pos
	ld a, (_sound_buffer_pos)
	inc a
	inc a
	ld (_sound_buffer_pos), a
	jp .timer_handler_sound_end

__endasm;
}

static uint8_t wave_ram_data[] = {
	0x0F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xF0, 0x00,
	0xFF, 0xFF, 0x0F, 0xFF, 0xF0, 0x00, 0x00, 0x00
};

/* static uint8_t wave_ram_data[] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00
}; */

/* static uint8_t wave_ram_data[] = {
	0xF0, 0x00, 0xF0, 0xFF, 0xFF, 0x0F, 0xF0, 0xF0,
	0xFF, 0xF0, 0xF0, 0x00, 0x00, 0x0F, 0x00, 0xF0
}; */

/* from hUGETracker - public domain */
/* static uint8_t wave_ram_data[] = {
	0x44, 0x1C, 0x5A, 0x43, 0xE3, 0x99, 0x76, 0xDD,
	0x3C, 0x98, 0x98, 0xCE, 0x97, 0xBE, 0xDC, 0x2A
}; */

void sound_init(void) BANKED {
	sound_enabled = true;
	sound_block_queueing = false;
	sound_buffer_len = 0;
	sound_is_playing = false;

	// configure wave RAM
	volatile uint8_t *wptr = (volatile uint8_t*) 0xFF30;
	for (uint8_t i = 0; i < 16; i++, wptr++) {
		*wptr = wave_ram_data[i];
	}
}

void sound_clear_queue(void) BANKED {
	CRITICAL {
		sound_buffer_len = 0;
		// timer will change this and reset sound hardware
		// sound_is_playing = false;
		// if it doesn't have to wait for a note to end
		sound_duration_counter = 0;
	}
}

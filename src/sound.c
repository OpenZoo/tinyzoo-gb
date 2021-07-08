#pragma bank 2

#include <string.h>
#include <gb/gb.h>
#include "timer.h"

uint8_t sound_buffer[254];
volatile uint8_t sound_buffer_pos;
volatile uint8_t sound_buffer_len;
volatile uint8_t sound_duration_counter;
bool sound_enabled;
bool sound_block_queueing;
bool sound_is_playing;
int8_t sound_current_priority;

void sound_update(void) {
__asm
		; sound playback routine
	push bc

	; sound playback routine
	ld a, (_sound_enabled)
	or a, a
	jp z, .timer_handler_no_sound
	ld a, (_sound_is_playing)
	or a, a
	jp z, .timer_handler_sound_end

	ld a, (_sound_duration_counter)
	dec a
	ld (_sound_duration_counter), a
	; _sound_duration_counter == 0
	jp z, .timer_handler_next_note
	; _sound_duration_counter < 0
	bit 7, a
	jp z, .timer_handler_sound_end

.timer_handler_next_note:
	ld a, (_sound_buffer_len)
	ld c, a
	ld a, (_sound_buffer_pos)
	cp a, c
	jp nc, .timer_handler_no_sound

	; init sound
	ld a, #0x80
	ldh (_NR52_REG + 0), a
	xor a, a
	ldh (_NR51_REG + 0), a
	ld a, #0xFF
	ldh (_NR50_REG + 0), a

	ld a, (_sound_buffer_pos)
	ld c, a
	ld b, #0x00 ; bc = sound_buffer_pos
	ld hl, #(_sound_buffer)
	add hl, bc ; hl = pointer to next entry

	ld a, (hl+) ; a = note
	push hl

.timer_handler_check_note:
	sub a, #16
	jp c, .timer_handler_check_drum
	cp a, #96
	jp nc, .timer_handler_check_drum
	add a, a 
	ld c, a
	ld b, #0x00
	ld hl, #(_sound_freqs)
	add hl, bc
	ld a, #0x80
	ldh (_NR21_REG + 0), a
	ld a, #0xF0
	ldh (_NR22_REG + 0), a
	ld a, (hl+)
	ldh (_NR23_REG + 0), a
	ld a, (hl)
	bit 7, a
	jp z, .timer_handler_checks_done ; dummy note
	ldh (_NR24_REG + 0), a
	ld a, #0x22
	ldh (_NR51_REG + 0), a
	jp .timer_handler_checks_done

.timer_handler_check_drum:
	; TODO: drum support

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

.timer_handler_no_sound:
	xor a, a
	ld (_sound_is_playing), a
	ldh (_NR52_REG + 0), a

.timer_handler_sound_end:
	pop		bc
__endasm;
}

void sound_init(void) BANKED {
	sound_enabled = true;
	sound_block_queueing = false;
	sound_buffer_len = 0;
	sound_is_playing = false;
}

void sound_clear_queue(void) BANKED {
	sound_buffer_len = 0;
	sound_is_playing = false;

	// TODO: reset sound hardware
}

void sound_queue(int8_t priority, const uint8_t *data) BANKED {
	if (!sound_block_queueing) {
		if (!sound_is_playing || (((priority >= sound_current_priority) && (sound_current_priority != -1)) || (priority == -1))) {
			uint8_t data_len = data[0];
			if (priority >= 0 || !sound_is_playing) {
				sound_current_priority = priority;
				__critical {
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
				__critical {
					sound_buffer_len -= pos;
					sound_buffer_pos = 0;
				};
				if ((sound_buffer_len + data_len) <= 254) {
					memcpy(sound_buffer + sound_buffer_len, data, data_len);
					sound_buffer_len += data_len;
				}
			}
			sound_is_playing = true;
		}
	}
}

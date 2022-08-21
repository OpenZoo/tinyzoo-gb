#pragma bank 2

#include <string.h>
#include <gbdk/platform.h>
#include "config.h"
#include "timer.h"

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

/* from hUGETracker - public domain */
static uint8_t wave_ram_data[] = {
	0x44, 0x1C, 0x5A, 0x43, 0xE3, 0x99, 0x76, 0xDD,
	0x3C, 0x98, 0x98, 0xCE, 0x97, 0xBE, 0xDC, 0x2A
};

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
	sound_buffer_len = 0;
	sound_is_playing = false;

	// TODO: reset sound hardware
}

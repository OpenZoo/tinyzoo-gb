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
	// no-op
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

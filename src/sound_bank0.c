#include <string.h>
#include <gb/gb.h>
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

void sound_queue_nobank(int8_t priority, const uint8_t *data) {
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
				__critical {
					sound_buffer_len -= sound_buffer_pos;
					memmove(sound_buffer, sound_buffer + sound_buffer_pos, sound_buffer_len);
					sound_buffer_pos = 0;
				};
				if ((sound_buffer_len + data_len) <= MAX_SOUND_BUFFER_SIZE) {
					memcpy(sound_buffer + sound_buffer_len, data, data_len);
					sound_buffer_len += data_len;
				}
			}
			sound_is_playing = true;
		}
	}
}

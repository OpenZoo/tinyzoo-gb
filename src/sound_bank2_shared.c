#pragma bank 2

#include <string.h>
#include <gbdk/platform.h>
#include "config.h"
#include "timer.h"

void sound_queue(int8_t priority, const uint8_t *data) BANKED {
	sound_queue_nobank(priority, data);
}

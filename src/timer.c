#include <gbdk/platform.h>
#include "timer.h"

uint16_t dhsecs;

uint16_t timer_hsecs(void) {
	return dhsecs >> 1;
}

bool timer_has_time_elapsed(uint16_t *ctr, uint16_t duration) {
	uint16_t hsecs_diff = (dhsecs >> 1) - *ctr;

	if (hsecs_diff >= duration) {
		*ctr = dhsecs >> 1;
		return true;
	} else {
		return false;
	}
}

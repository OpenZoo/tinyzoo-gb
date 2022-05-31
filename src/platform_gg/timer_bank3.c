#pragma bank 3

#include <gbdk/platform.h>
#include "timer.h"

extern uint16_t dhsecs;

void timer_init(void) BANKED {
	// TODO
	dhsecs = 0;
}

#pragma bank 3

#include <gbdk/platform.h>
#include "timer.h"

extern uint16_t dhsecs;
extern uint8_t timer_state;
extern uint8_t timer_speed_base;
extern uint8_t timer_speed_drum;

void timer_init(void) BANKED {
	dhsecs = 0;
	timer_state = 0;

	if (_cpu == CGB_TYPE) {
		timer_speed_base = 30; // 256 - 226
		timer_speed_drum = 248; // 256 - 8
	} else {
		timer_speed_base = 143; // 256 - 113
		timer_speed_drum = 252; // 256 - 4
		if (sgb_check() && _cpu == DMG_TYPE) {
			// SGB 1 speed workaround
			timer_speed_base = 141; // 256 - 115
		}
	}

	TMA_REG = timer_speed_base;
	TAC_REG = 0x04;
	IE_REG |= TIM_IFLAG;
}

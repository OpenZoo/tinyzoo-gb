#include <gb/gb.h>
#include "timer.h"

extern uint16_t dhsecs;
extern uint8_t timer_state;

void sound_queue(int8_t priority, const uint8_t *data) {
	// TODO
}

void timer_init(void) {
	dhsecs = 0;
	timer_state = 0;
	uint8_t tmr_modulo = (_cpu == CGB_TYPE) ? 226 : 113;
	TMA_REG = 255 - tmr_modulo;
	TAC_REG = 0x04;
	IE_REG |= TIM_IFLAG;
}

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
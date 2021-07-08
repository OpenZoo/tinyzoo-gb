#include <gb/gb.h>
#include "input.h"

int8_t input_delta_x, input_delta_y;
bool input_shift_pressed;

void input_update(void) {
	uint8_t keys = joypad();

	if (keys & J_UP) {
		input_delta_x = 0;
		input_delta_y = -1;
	} else if (keys & J_DOWN) {
		input_delta_x = 0;
		input_delta_y = 1;
	} else if (keys & J_LEFT) {
		input_delta_x = -1;
		input_delta_y = 0;
	} else if (keys & J_RIGHT) {
		input_delta_x = 1;
		input_delta_y = 0;
	} else {
		input_delta_x = 0;
		input_delta_y = 0;
	}

	input_shift_pressed = (keys & J_A) ? 1 : 0;
}
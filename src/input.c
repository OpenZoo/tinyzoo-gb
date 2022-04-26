#include <gb/gb.h>
#include "input.h"

#define JOY_REPEAT_DELAY 15
#define JOY_REPEAT_DELAY_NEXT 2

extern uint8_t vbl_ticks;

static uint8_t input_keys = 0;
static uint8_t input_keys_released = 0;
uint8_t input_pressed;
static uint8_t input_vbls_next[4];
static uint8_t input_last = 0;

int8_t input_delta_x, input_delta_y;

void input_update_vbl(void) {
	uint8_t keys = joypad();
	input_keys |= keys;
	input_keys_released |= (input_pressed & (~keys));
}

void input_update(void) {
	uint8_t keys;
	CRITICAL {
		keys = input_keys;
		input_keys = 0;
		input_pressed &= ~input_keys_released;
		input_keys_released = 0;
	}

	input_delta_x = 0;
	input_delta_y = 0;

	input_pressed &= 0x0F;
	input_pressed |= (keys & 0xF0);
	if (keys & 0x0F) for (uint8_t i = 0; i < 4; i++) {
		uint8_t input_id = (input_last + 1) & 3;
		uint8_t input_mask = 1 << input_id;
		input_last = input_id;

		if (keys & input_mask) {
			if (input_pressed & input_mask) {
				if (((uint8_t) (input_vbls_next[input_id] - vbl_ticks)) < 0x80) continue;
				input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY_NEXT;
			} else {
				input_pressed |= input_mask;
				input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY;
			}
			if (input_mask == J_UP) {
				input_delta_y = -1;
			} else if (input_mask == J_DOWN) {
				input_delta_y = 1;
			} else if (input_mask == J_LEFT) {
				input_delta_x = -1;
			} else if (input_mask == J_RIGHT) {
				input_delta_x = 1;
			}
			break;
		}
	}
}

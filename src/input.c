#include <gb/gb.h>
#include "input.h"

//#define JOY_REPEAT_DELAY 15
#define JOY_REPEAT_DELAY 11
#define JOY_REPEAT_DELAY_NEXT 2

extern uint8_t vbl_ticks;

static uint8_t input_keys = 0;
static uint8_t input_keys_repressed = 0;
static uint8_t input_keys_released = 0;
uint8_t input_held;
static uint8_t input_vbls_next[4];
static uint8_t input_last = 0;

int8_t input_delta_x, input_delta_y;

void input_update_vbl(void) {
	uint8_t keys = joypad();
	input_keys |= keys;
	input_keys_repressed |= (keys & input_keys_released);
	input_keys_released |= (input_held & (~keys));
}

void input_update(void) {
	uint8_t keys_pressed;
	uint8_t keys_repressed;
	uint8_t keys_released;

	CRITICAL {
		keys_pressed = input_keys;
		keys_repressed = input_keys_repressed;
		keys_released = input_keys_released;
	}

	input_delta_x = 0;
	input_delta_y = 0;

	input_held = (input_held & 0x0F) | (keys_pressed & 0xF0);
	if (keys_pressed & 0x0F) for (uint8_t i = 0; i < 4; i++) {
		uint8_t input_id = (input_last + 1) & 3;
		uint8_t input_mask = 1 << input_id;
		input_last = input_id;

		if (keys_pressed & input_mask) {
			if (keys_repressed & input_mask) {
				goto KeyRepressed;
			} else if (input_held & input_mask) {
				if (keys_released & input_mask) {
					input_held &= ~input_mask;
				}
				if (((uint8_t) (input_vbls_next[input_id] - vbl_ticks)) < 0x80) continue;
				if (!(keys_released & input_mask)) {
					input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY_NEXT;
				}
			} else {
				if (!(keys_released & input_mask)) {
KeyRepressed:
					input_held |= input_mask;
					input_vbls_next[input_id] = vbl_ticks + JOY_REPEAT_DELAY;
				}
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

	CRITICAL {
		input_keys = 0;
		input_keys_repressed = 0;
		input_keys_released = 0;
		input_update_vbl();
	}
}

#include "game.h"

#include <gb/gb.h>
#include "config.h"
#include "game_transition.h"

#include "../res/game_transition_table.inc"

// must be a divisor of the table size
#define GAME_TRANSITION_STEP 1

static uint16_t transition_pos = 0xFFFF;
static uint8_t transition_color;

extern int8_t viewport_x;
extern int8_t viewport_y;

void game_transition_start(uint8_t color) {
	while (transition_pos < (TRANSITION_TABLE_20_17_ENTRY_COUNT * 2)) {
__asm
		halt
__endasm;
	}
	transition_pos = 0;
	transition_color = color;
}

void game_transition_step(void) {
	if (transition_pos >= (TRANSITION_TABLE_20_17_ENTRY_COUNT * 2)) return;

	uint8_t len = GAME_TRANSITION_STEP;
	const uint8_t *arr = transition_table_20_17 + transition_pos;

	if (transition_color == 0xFF) {
		while ((len--) > 0) {
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
	} else {
		while ((len--) > 0) {
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			text_draw(x, y, 178, transition_color);
		}
	}
	transition_pos += GAME_TRANSITION_STEP * 2;
}

bool game_transition_running(void) {
	return transition_pos < (TRANSITION_TABLE_20_17_ENTRY_COUNT * 2);
}

/* void game_transition_tick_to_end(void) BANKED {
	while (transition_pos != TRANSITION_TABLE_20_17_ENTRY_COUNT) {
		game_transition_step();
	}
} */
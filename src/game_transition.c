#include "game.h"

#include <gbdk/platform.h>
#include "bank_switch.h"
#include "config.h"
#include "game_transition.h"

#ifdef FEAT_BOARD_TRANSITIONS

// #define TRANSITION_TABLE_20_17_ENTRY_COUNT 340
// extern const uint8_t transition_table_20_17[];
#define TRANSITION_TABLE_20_18_ENTRY_COUNT 360
extern const uint8_t transition_table_20_18[];

// must be a divisor of the table size

static uint16_t transition_pos = 0xFFFF;
static uint8_t transition_color;

extern int8_t viewport_x;
extern int8_t viewport_y;

void game_transition_start(uint8_t color) {
	while (transition_pos < (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2)) {
__asm
		halt
__endasm;
	}
	transition_pos = 0;
	transition_color = color;
}

void game_transition_step(void) {
	if (transition_pos >= (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2)) return;

	const uint8_t *arr = transition_table_20_18 + transition_pos;
	uint8_t prev_bank = _current_bank;
	ZOO_SWITCH_ROM(3);

	if (transition_color == 0xFF) {
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		{
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			board_draw_tile(viewport_x + x, viewport_y + y);
		}
		transition_pos += 12;
	} else {
		for (uint8_t i = 0; i < 7; i++) {
			uint8_t x = *(arr++);
			uint8_t y = *(arr++);
			text_draw(x, y, 178, transition_color);
		}
		transition_pos += 14;
	}

	ZOO_SWITCH_ROM(prev_bank);
}

bool game_transition_running(void) {
	return transition_pos < (TRANSITION_TABLE_20_18_ENTRY_COUNT * 2);
}

#endif

/* void game_transition_tick_to_end(void) BANKED {
	while (transition_pos != TRANSITION_TABLE_20_18_ENTRY_COUNT) {
		game_transition_step();
	}
} */

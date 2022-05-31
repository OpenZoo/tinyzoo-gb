#pragma bank 3
// Must be in the same bank as the "small" font and message_consts.

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gbdk/platform.h>
#include "gamevars.h"
#include "../renderer_sidebar.h"
#include "../../res/font_small.h"

void sidebar_set_message_color(uint8_t color) BANKED {
	// TODO
}

void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED {
	// TODO
}

void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED {
	// TODO
}

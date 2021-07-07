#include <stdint.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "../res/font_default.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "timer.h"

#include "board_manager.h"
#include "town_bin.h"

#ifdef PROFILING
#include <gb/bgb_emu.h>
#endif

void main(void) {
	srand(1);

	world_create();
	load_board_data(_TOWN_BIN);

	if (_cpu == CGB_TYPE) {
		cpu_fast();
		text_init(&renderer_gbc);
	} else {
		text_init(&renderer_dmg);
	}

	timer_init();

	game_play_loop(true);

	while (1);
}

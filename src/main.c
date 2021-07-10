#include <stdint.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "../res/font_default.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "timer.h"

#include "board_manager.h"

#ifdef PROFILING
#include <gb/bgb_emu.h>
#endif

void main(void) {
	srand(1);

	sound_init();

	if (_cpu == CGB_TYPE) {
		cpu_fast();
		text_init(&renderer_gbc);
	} else {
		text_init(&renderer_dmg);
	}

	world_create();

	load_world(0);
	load_board(zoo_world_info.current_board);

	timer_init();

	board_enter();

	game_play_loop(true);

	while (1);
}

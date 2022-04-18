#pragma bank 3

#include <stdint.h>
#include <gb/gb.h>
#include <gb/cgb.h>
#include "../res/font_default.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "sram_debug.h"
#include "timer.h"

#include "board_manager.h"

void init_main(void) BANKED {
	srand(1);

	sound_init();

	if (_cpu == CGB_TYPE) {
		cpu_fast();
		text_init(RENDER_MODE_PLAYFIELD, &renderer_gbc);
	} else {
		text_init(RENDER_MODE_PLAYFIELD, &renderer_dmg);
	}

	sram_init(false);

	world_create();

	load_world(0);
	load_board(zoo_world_info.current_board);
	zoo_game_state.paused = true; // TODO: not here...

	timer_init();

	board_enter_stage1();
	board_enter_stage2();
	board_enter_stage3();
}

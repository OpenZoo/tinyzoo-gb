#pragma bank 3

#include <stdint.h>
#include <gbdk/platform.h>
#include "../res/font_default.h"
#include "game.h"
#include "math.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "timer.h"

#include "board_manager.h"

void init_main(void) BANKED {
	wait_vbl_done();
#ifdef NINTENDO
	LCDC_REG = 0x00;
#endif

	srand(1);

	sound_init();

#if defined(__POCKET__)
	cpu_fast();
	text_init(RENDER_MODE_PLAYFIELD, &renderer_gbc);
#elif defined(GAMEBOY)
	if (_cpu == CGB_TYPE) {
		cpu_fast();
		text_init(RENDER_MODE_PLAYFIELD, &renderer_gbc);
	} else {
		text_init(RENDER_MODE_PLAYFIELD, &renderer_dmg);
	}
#else
	text_init(RENDER_MODE_PLAYFIELD);
#endif

	sram_init(false);

	world_create();

	zoo_game_state.world_id = 0;

	timer_init();

	game_menu_act_enter_world(zoo_game_state.world_id, false);
}

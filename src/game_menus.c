#pragma bank 2

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gbdk/platform.h>
#include "../res/menu_entry_consts.h"
#include "board_manager.h"
#include "config.h"
#include "elements.h"
#include "game.h"
#include "input.h"
#include "txtwind.h"

static void board_pause_enter(void) {
	zoo_game_state.paused = true; // TODO: not here...

	board_enter_stage1();
	board_enter_stage2();
	board_enter_stage3();
}

void game_menu_act_enter_world(uint8_t world_id, bool new_game, bool first_launch) BANKED {
	load_world(world_id, new_game);
	uint8_t starting_board = zoo_world_info.current_board;

#ifdef SHOW_TITLE
	if (first_launch) {
		load_board(0);
		viewport_reset();

		zoo_game_state.game_state_element = E_MONITOR;
		zoo_game_state.paused = false;
		game_play_loop(true);
		while (input_start_pressed) input_update();
	}
#endif

	zoo_game_state.game_state_element = E_PLAYER;
	load_board(starting_board);
	board_pause_enter();
}

bool game_pause_menu(void) BANKED {
	if (zoo_world_info.health <= 0) {
		// "Game Over" menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_new_game, 3);
		txtwind_append((uint16_t) menu_entry_continue, 3);
		switch (txtwind_run(RENDER_MODE_MENU)) {
		case 0: { /* NEW GAME */
			game_menu_act_enter_world(zoo_game_state.world_id, true, false);
			return true;
		} break;
		case 1: { /* CONTINUE */
			clear_saved_board(zoo_world_info.current_board);
			game_menu_act_enter_world(zoo_game_state.world_id, false, false);
			return true;
		} break;
		}
	} else {
		// Regular menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_continue, 3);
		txtwind_append((uint16_t) menu_entry_restart, 3);
		switch (txtwind_run(RENDER_MODE_MENU)) {
		case 1: { /* RESTART */
			game_menu_act_enter_world(zoo_game_state.world_id, true, false);
			return true;
		} break;
		}
	}

	return false;
}

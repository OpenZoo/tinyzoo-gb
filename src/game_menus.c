#pragma bank 2

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gbdk/platform.h>
#include "../res/menu_entry_consts.h"
#include "board_manager.h"
#include "game.h"
#include "txtwind.h"

static void board_pause_enter(void) {
	zoo_game_state.paused = true; // TODO: not here...

	board_enter_stage1();
	board_enter_stage2();
	board_enter_stage3();
}

void game_menu_act_enter_world(uint8_t world_id, bool new_game) BANKED {
	load_world(world_id, new_game);
	load_board(zoo_world_info.current_board);

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
			game_menu_act_enter_world(zoo_game_state.world_id, true);
			return true;
		} break;
		case 1: { /* CONTINUE */
			clear_saved_board(zoo_world_info.current_board);
			game_menu_act_enter_world(zoo_game_state.world_id, false);
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
			game_menu_act_enter_world(zoo_game_state.world_id, true);
			return true;
		} break;
		}
	}

	return false;
}

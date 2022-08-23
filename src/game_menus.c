#pragma bank 2

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

		while (input_keys != 0) {
			input_reset();
			wait_vbl_done();
		}

		sound_clear_queue();
	}
#endif

	zoo_game_state.game_state_element = E_PLAYER;
	load_board(starting_board);
	board_pause_enter();
}

static void game_menu_act_about(void) {
	txtwind_open_license();
	txtwind_run(RENDER_MODE_TXTWIND);
}

bool game_pause_menu(void) BANKED {
MenuStart:
	if (zoo_world_info.health <= 0) {
		// "Game Over" menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_new_game, 3);
		txtwind_append((uint16_t) menu_entry_continue, 3);
		txtwind_append((uint16_t) menu_entry_about, 3);
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
		case 2: { /* ABOUT */
			game_menu_act_about();
			goto MenuStart;
		} break;
		}
	} else {
		// Regular menu
		txtwind_init();
		txtwind_append((uint16_t) menu_entry_continue, 3);
		txtwind_append((uint16_t) menu_entry_restart, 3);
		txtwind_append((uint16_t) menu_entry_about, 3);
		switch (txtwind_run(RENDER_MODE_MENU)) {
		case 1: { /* RESTART */
			game_menu_act_enter_world(zoo_game_state.world_id, true, false);
			return true;
		} break;
		case 2: { /* ABOUT */
			game_menu_act_about();
			goto MenuStart;
		} break;
		}
	}

	return false;
}

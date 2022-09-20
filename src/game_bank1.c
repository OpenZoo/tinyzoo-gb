#pragma bank 1

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
#include "cheat_detect.h"
#include "gamevars.h"
#include "game.h"
#include "elements.h"
#include "input.h"
#include "math.h"
#include "oop.h"
#include "renderer.h"
#include "renderer_sidebar.h"
#include "sound_consts.h"
#include "timer.h"

void board_attack(uint8_t stat_id, uint8_t x, uint8_t y) {
	if (stat_id == 0 && zoo_world_info.energizer_ticks > 0) {
		zoo_world_info.score += zoo_element_defs_scorevalues[ZOO_TILE(x, y).element];
		game_update_sidebar_score();
	} else {
		damage_stat(stat_id);
	}

	if (stat_id > 0 && stat_id <= zoo_game_state.current_stat_ticked) {
		zoo_game_state.current_stat_ticked--;
	}

	if ((ZOO_TILE(x, y).element == E_PLAYER) && zoo_world_info.energizer_ticks > 0) {
		zoo_stat_t *attacker_stat = &ZOO_STAT(stat_id);
		zoo_world_info.score += zoo_element_defs_scorevalues[ZOO_TILE(attacker_stat->x, attacker_stat->y).element];
		game_update_sidebar_score();
	} else {
		board_damage_tile(x, y);
		sound_queue(2, sound_damage);
	}
}

static void game_play_handle_pause(bool pause_blink) {
	uint8_t px = ZOO_STAT(0).x;
	uint8_t py = ZOO_STAT(0).y;

	if (pause_blink) {
		board_draw_char(px, py, 0x02, 0x1F); // player color/char
	} else {
		if (ZOO_TILE(px, py).element == E_PLAYER) {
			board_draw_char(px, py, ' ', 0x0F);
		} else {
			board_draw_tile(px, py);
		}
	}

	if (input_delta_x != 0 || input_delta_y != 0) {
		uint8_t mpx = px + input_delta_x;
		uint8_t mpy = py + input_delta_y;
		zoo_element_defs_touchprocs[ZOO_TILE(mpx, mpy).element](mpx, mpy, &input_delta_x, &input_delta_y);
		px = ZOO_STAT(0).x;
		py = ZOO_STAT(0).y;
		mpx = px + input_delta_x;
		mpy = py + input_delta_y;
		if ((mpx != 0 || mpy != 0) && (zoo_element_defs_flags[ZOO_TILE(mpx, mpy).element] & ELEMENT_WALKABLE)) {
			if (ZOO_TILE(px, py).element == E_PLAYER) {
				move_stat(0, mpx, mpy);
			} else {
				board_draw_tile(px, py);
				ZOO_STAT(0).x = mpx;
				ZOO_STAT(0).y = mpy;
				ZOO_TILE_CHANGE2(mpx, mpy, E_PLAYER, zoo_element_defs_color[E_PLAYER]);
				board_draw_tile(mpx, mpy);

				// center_viewport_on_player();
				// board_redraw();
				if (viewport_request_player_focus()) {
					move_stat_scroll_focused(0, px, py, mpx, mpy, true);
				}
				text_update();
			}

			zoo_game_state.paused = false;
			zoo_game_state.current_tick = rand_mod(100);
			zoo_game_state.current_stat_ticked = zoo_stat_count + 1;
		}
	} else {
		// Save power
		wait_vbl_done();
	}
}

void game_play_loop(bool board_changed) BANKED {
RestartGameLoop:
	sound_block_queueing = false;

	game_update_sidebar_all();

	ZOO_TILE_CHANGE2(ZOO_STAT(0).x, ZOO_STAT(0).y,
		zoo_game_state.game_state_element,
		zoo_element_defs_color[zoo_game_state.game_state_element]
	);

	if (board_changed) {
		board_redraw();
		text_reinit(zoo_game_state.game_state_element == E_MONITOR
			? RENDER_MODE_TITLE
			: RENDER_MODE_PLAYFIELD);
		text_update();
	}

	zoo_game_state.tick_time_duration = 8;
	zoo_game_state.play_exit_requested = false;

	zoo_game_state.current_tick = rand_mod(100);
	zoo_game_state.current_stat_ticked = zoo_stat_count + 1;

	bool pause_blink;

	do {
		if (zoo_game_state.paused) {
			if (timer_has_time_elapsed(&zoo_game_state.tick_time_counter, 25)) {
				pause_blink = !pause_blink;
			}
			input_update();
			if (input_start_pressed) {
				if (game_pause_menu()) {
					board_changed = true;
					goto RestartGameLoop;
				}
			}
			game_play_handle_pause(pause_blink);
		} else if (zoo_game_state.current_stat_ticked <= zoo_stat_count) {
			zoo_stat_t *stat = &ZOO_STAT(zoo_game_state.current_stat_ticked);
			if (ZOO_TILE_READBOUNDS(stat->x, stat->y)) {
				uint8_t element = ZOO_TILE(stat->x, stat->y).element;

				if (zoo_element_defs_tickprocs[element] != 0 && stat->cycle != 0) {
					// if (stat->cycle == 1 || ((zoo_game_state.current_tick % stat->cycle) == (zoo_game_state.current_stat_ticked % stat->cycle))) {
					if (stat->cycle == 1 || zoo_mods16_8(((int16_t) zoo_game_state.current_tick - zoo_game_state.current_stat_ticked), stat->cycle) == 0) {
						zoo_element_defs_tickprocs[element](zoo_game_state.current_stat_ticked);
					}
				}
			}

			zoo_game_state.current_stat_ticked++;
		} else if (zoo_game_state.current_stat_ticked > zoo_stat_count && !zoo_game_state.play_exit_requested) {
			text_update();

			ZOO_BUSYLOOP(!timer_has_time_elapsed(&zoo_game_state.tick_time_counter, zoo_game_state.tick_time_duration));

			if (zoo_game_state.current_tick >= 420) zoo_game_state.current_tick = 1;
			else zoo_game_state.current_tick++;

			zoo_game_state.current_stat_ticked = 0;

			input_update();
#ifdef SHOW_CHEATS
			cheat_detect_update();
#endif
			if (input_start_pressed) {
				if (zoo_game_state.game_state_element == E_MONITOR) {
					return;
				} else if (game_pause_menu()) {
					board_changed = true;
					goto RestartGameLoop;
				}
			} else if (input_select_pressed) {
				if (zoo_game_state.game_state_element != E_MONITOR) {
					game_scrolling_view();
				}
			}
		}
	} while(!zoo_game_state.play_exit_requested);
}

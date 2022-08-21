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

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../game_transition.h"
#include "../math.h"
#include "../renderer_sidebar.h"
#include "../sound_consts.h"
#include "../timer.h"

static const zoo_tile_t empty_tile = {E_EMPTY, 0};

void ElementPassageTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	uint8_t col = ZOO_TILE(x, y).color;
	uint8_t old_board = zoo_world_info.current_board;

#ifdef FEAT_BOARD_TRANSITIONS
	game_transition_board_change_start();
	sound_queue(4, sound_passage_teleport); // gbzoo: moved from near board_enter() to here
#endif
	sidebar_hide_message();
	board_change(ZOO_STAT_AT(x, y).p3);

	uint8_t new_x = 0, new_y;
	for (uint8_t ix = 1; ix <= BOARD_WIDTH; ix++) {
		for (uint8_t iy = 1; iy <= BOARD_HEIGHT; iy++) {
			zoo_tile_t tile;
			ZOO_TILE_ASSIGN(tile, ix, iy);
			if (tile.element == E_PASSAGE && tile.color == col) {
				new_x = ix;
				new_y = iy;
			}
		}
	}

	ZOO_TILE_COPY(ZOO_TILE(ZOO_STAT(0).x, ZOO_STAT(0).y), empty_tile);
	if (new_x != 0) {
		ZOO_STAT(0).x = new_x;
		ZOO_STAT(0).y = new_y;
	}

	zoo_game_state.paused = true;
#ifdef FEAT_BOARD_TRANSITIONS
	board_enter_stage1();
	game_transition_board_change_end();
	board_enter_stage2();
	ZOO_BUSYLOOP(game_transition_running());
	board_enter_stage3();
#else
	sound_queue(4, sound_passage_teleport);
	board_enter_stage1();
	board_enter_stage2();
	board_redraw();
	board_enter_stage3();
#endif

	*dx = 0;
	*dy = 0;
}

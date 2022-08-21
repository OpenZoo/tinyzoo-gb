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
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static void ElementConveyorTick(uint8_t x, uint8_t y, int8_t dir) {
	uint8_t i_min, i_max;
	if (dir == 1) {
		i_min = 0;
		i_max = 8;
	} else {
		i_min = 7;
		i_max = 255;
	}

	bool can_move = true;
	uint8_t tx, ty;
	zoo_tile_t tiles[8];
	uint8_t stat_ids[8];

	// Moved from CWTick/CCWTick
	board_draw_tile(x, y);

	for (uint8_t i = i_min; i != i_max; i += dir) {
		tx = x + diagonal_delta_x[i];
		ty = y + diagonal_delta_y[i];
		zoo_tile_t ttile;
		ZOO_TILE_COPY(ttile, ZOO_TILE(tx, ty));
		ZOO_TILE_COPY(tiles[i], ttile);
		if (ttile.element == E_EMPTY) {
			can_move = true;
		} else if (!(zoo_element_defs_flags[ttile.element] & ELEMENT_PUSHABLE)) {
			can_move = false;
		}

		if (zoo_element_defs_flags[ttile.element] & ELEMENT_TYPICALLY_STATTED) {
			stat_ids[i] = get_stat_id_at(tx, ty);
		}
	}

	for (uint8_t i = i_min; i != i_max; i += dir) {
		zoo_tile_t ttile;
		ZOO_TILE_COPY(ttile, tiles[i]);

		if (can_move) {
			if (zoo_element_defs_flags[ttile.element] & ELEMENT_PUSHABLE) {
				uint8_t ix = x + diagonal_delta_x[(i - dir) & 7];
				uint8_t iy = y + diagonal_delta_y[(i - dir) & 7];
				tx = x + diagonal_delta_x[i];
				ty = y + diagonal_delta_y[i];

				if (zoo_element_defs_flags[ttile.element] & ELEMENT_TYPICALLY_STATTED) {
					zoo_tile_t tmp_tile;
					ZOO_TILE_COPY(tmp_tile, ZOO_TILE(tx, ty));
					ZOO_TILE_COPY(ZOO_TILE(tx, ty), ttile);
					ZOO_TILE(ix, iy).element = E_EMPTY;
					move_stat(stat_ids[i], ix, iy);

					ZOO_TILE_COPY(ZOO_TILE(tx, ty), tmp_tile);
					board_draw_tile(tx, ty);
				} else {
					ZOO_TILE_COPY(ZOO_TILE(ix, iy), ttile);					
					board_draw_tile(ix, iy);
				}

				if (!(zoo_element_defs_flags[tiles[(i + dir) & 7].element] & ELEMENT_PUSHABLE)) {
					ZOO_TILE(tx, ty).element = E_EMPTY;
					board_draw_tile(tx, ty);
				}
			} else {
				can_move = false;
			}
		} else if (ttile.element == E_EMPTY) {
			can_move = true;
		} else if (!(zoo_element_defs_flags[ttile.element] & ELEMENT_PUSHABLE)) {
			can_move = false;
		}
	}
}

static const uint8_t conveyor_cw_tiles[4] = {179, 47, 196, 92};

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick / 3) & 3)];
}

void ElementConveyorCWTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	ElementConveyorTick(stat->x, stat->y, 1);
}

uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick >> 1) & 3) ^ 3];
}

void ElementConveyorCCWTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	ElementConveyorTick(stat->x, stat->y, -1);
}

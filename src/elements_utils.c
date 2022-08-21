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

#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "math.h"
#include "oop.h"
#include "sound_consts.h"
#include "timer.h"

void ElementMove(uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y) {
	uint8_t stat_id = get_stat_id_at(old_x, old_y);
	if (stat_id != STAT_ID_NONE) {
		move_stat(stat_id, new_x, new_y);
	} else {
		if (ZOO_TILE_WRITEBOUNDS(new_x, new_y)) {
			ZOO_TILE_COPY(ZOO_TILE(new_x, new_y), ZOO_TILE(old_x, old_y));
			board_draw_tile(new_x, new_y);
		}
		if (ZOO_TILE_WRITEBOUNDS(old_x, old_y)) {
			ZOO_TILE(old_x, old_y).element = E_EMPTY;
			board_draw_tile(old_x, old_y);
		}
	}
}

void ElementPushablePush(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	ZOO_TILE_ASSIGN(tile, x, y);
	if (((tile.element == E_SLIDER_NS) && (dx == 0)) || ((tile.element == E_SLIDER_EW) && (dy == 0))
		|| (zoo_element_defs_flags[tile.element] & ELEMENT_PUSHABLE)) {

		zoo_tile_t dtile;
		ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		if (dtile.element == E_TRANSPORTER) {
			ElementTransporterMove(x, y, dx, dy);
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		} else if (dtile.element != E_EMPTY) {
			if (dx != 0 || dy != 0) {
				ElementPushablePush(x + dx, y + dy, dx, dy);
				ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
			}
		}

		if (!(zoo_element_defs_flags[dtile.element] & ELEMENT_WALKABLE)
			&& (zoo_element_defs_flags[dtile.element] & ELEMENT_DESTRUCTIBLE)
			&& (dtile.element != E_PLAYER))
		{
			board_damage_tile(x + dx, y + dy);
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		}

		if (zoo_element_defs_flags[dtile.element] & ELEMENT_WALKABLE) {
			ElementMove(x, y, x + dx, y + dy);
		}
	}
}

void ElementTransporterMove(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	zoo_stat_t *stat = &ZOO_STAT_AT(x + dx, y + dy);
	if ((stat->step_x == dx) && (stat->step_y == dy)) {
		int8_t ix = stat->x;
		int8_t iy = stat->y;
		bool is_valid_dest = true;
		while (1) {
			ix += dx;
			iy += dy;
			ZOO_TILE_ASSIGN(tile, ix, iy);
			if (tile.element == E_BOARD_EDGE) {
				break;
			} else if (is_valid_dest) {
				is_valid_dest = false;

				if (!(zoo_element_defs_flags[tile.element] & ELEMENT_WALKABLE)) {
					ElementPushablePush(ix, iy, dx, dy);
					ZOO_TILE_ASSIGN(tile, ix, iy);
				}

				if (zoo_element_defs_flags[tile.element] & ELEMENT_WALKABLE) {
					ElementMove(stat->x - dx, stat->y - dy, ix, iy);
					sound_queue(3, sound_transporter_move);
					break;
				}
			}

			if (tile.element == E_TRANSPORTER) {
				zoo_stat_t *stat2 = &ZOO_STAT_AT(ix, iy);
				if (stat2->step_x == -dx && stat2->step_y == -dy) {
					is_valid_dest = true;
				}
			}
		}
	}
}

void DrawPlayerSurroundings(uint8_t x, uint8_t y, uint8_t bomb_phase) {
	for (int8_t ix = x - TORCH_DX - 1; ix <= x + TORCH_DX + 1; ix++) {
		if (ix < 1) continue;
		if (ix > BOARD_WIDTH) break;
		for (int8_t iy = y - TORCH_DY - 1; iy <= y + TORCH_DY + 1; iy++) {
			if (iy < 1) continue;
			if (iy > BOARD_HEIGHT) break;

			if (bomb_phase > 0) {
				int8_t dx = ix - x;
				int8_t dy = iy - y;
				int16_t dist = (int16_t)(dx*dx) + ((int16_t)(dy*dy) << 1);
				if (dist < TORCH_DIST_SQR) {
					zoo_tile_t *tile = &ZOO_TILE(ix, iy);
					if (bomb_phase == 1) {
						if (zoo_element_defs_flags[tile->element] & ELEMENT_TYPICALLY_TEXTED) {
							uint8_t i_stat = get_stat_id_at(ix, iy);
							if (i_stat > 0) {
								oop_send(i_stat, true, OOP_LABEL_BOMBED, false);
							}
						}

						if (tile->element == E_STAR || (zoo_element_defs_flags[tile->element] & ELEMENT_DESTRUCTIBLE)) {
							board_damage_tile(ix, iy);
						}

						if (tile->element == E_EMPTY || tile->element == E_BREAKABLE) {
							tile->element = E_BREAKABLE;
							tile->color = 9 + rand(7);
						}
					} else {
						if (tile->element == E_BREAKABLE) {
							tile->element = E_EMPTY;
						}
					}
				}
			}
			board_draw_tile(ix, iy);
		}
	}
}

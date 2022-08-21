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
#include "../oop.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementBulletTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	bool first_try = true;
	uint8_t ix, iy, ielem;
	
TryMove:
	ix = stat->x + stat->step_x;
	iy = stat->y + stat->step_y;
	ielem = ZOO_TILE_READBOUNDS(ix, iy) ? ZOO_TILE(ix, iy).element : 0;

	if ((zoo_element_defs_flags[ielem] & ELEMENT_WALKABLE) || (ielem == E_WATER)) {
		move_stat(stat_id, ix, iy);
		return;
	}

	if ((ielem == E_RICOCHET) && first_try) {
		stat->step_x = -(stat->step_x);
		stat->step_y = -(stat->step_y);
		sound_queue(1, sound_ricochet);
		first_try = false;
		goto TryMove;
	}

	if ((ielem == E_BREAKABLE) || (
		(zoo_element_defs_flags[ielem] & ELEMENT_DESTRUCTIBLE)
		&& ((ielem == E_PLAYER) || (stat->p1 == 0))
	)) {
		if (zoo_element_defs_scorevalues[ielem] != 0) {
			zoo_world_info.score += zoo_element_defs_scorevalues[ielem];
			game_update_sidebar_score();
		}
		board_attack(stat_id, ix, iy);
		return;
	}

	if (first_try) {
		uint8_t relem = ZOO_TILE(stat->x + stat->step_y, stat->y + stat->step_x).element;
		if (relem == E_RICOCHET) {
			ix = stat->step_x;
			stat->step_x = -stat->step_y;
			stat->step_y = -ix;
			sound_queue(1, sound_ricochet);
			first_try = false;
			goto TryMove;
		}

		relem = ZOO_TILE(stat->x - stat->step_y, stat->y - stat->step_x).element;
		if (relem == E_RICOCHET) {
			ix = stat->step_x;
			stat->step_x = stat->step_y;
			stat->step_y = ix;
			sound_queue(1, sound_ricochet);
			first_try = false;
			goto TryMove;
		}
	}

	remove_stat(stat_id);
	zoo_game_state.current_stat_ticked--;
	if ((ielem == E_OBJECT) || (ielem == E_SCROLL)) {
		uint8_t i_stat = get_stat_id_at(ix, iy);
		oop_send(i_stat, true, OOP_LABEL_SHOT, false);
	}
}

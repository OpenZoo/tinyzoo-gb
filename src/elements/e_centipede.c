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

#define STAT_ID_UNUSED 255
#define STAT_ID_UNUSED_STAGE2 254
#define STAT_ID_VALID(id) ((id) < STAT_ID_UNUSED_STAGE2)

// WARNING: No READBOUNDS
void ElementCentipedeHeadTick(uint8_t stat_id) {
	uint8_t tmp;
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	uint8_t head_x = stat->x;
	uint8_t head_y = stat->y;

	if (head_x == ZOO_STAT(0).x && (rand(10) < stat->p1)) {
		stat->step_y = signum8(ZOO_STAT(0).y - head_y);
		stat->step_x = 0;
	} else if (head_y == ZOO_STAT(0).y && (rand(10) < stat->p1)) {
		stat->step_x = signum8(ZOO_STAT(0).x - head_x);
		stat->step_y = 0;
	} else if (((rand(10) << 2) < stat->p2) || (stat->step_x == 0 && stat->step_y == 0)) {
		calc_direction_rnd(&stat->step_x, &stat->step_y);
	}
	
	uint8_t elem = ZOO_TILE(head_x + stat->step_x, head_y + stat->step_y).element;
	if (!(zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
		int8_t ix = stat->step_x;
		int8_t iy = stat->step_y;
		tmp = ((RAND2() << 1) - 1) * iy;
		stat->step_y = ((RAND2() << 1) - 1) * ix;
		stat->step_x = tmp;
		
		elem = ZOO_TILE(head_x + stat->step_x, head_y + stat->step_y).element;
		if (!(zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
			stat->step_x = -stat->step_x;
			stat->step_y = -stat->step_y;

			elem = ZOO_TILE(head_x + stat->step_x, head_y + stat->step_y).element;
			if (!(zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
				elem = ZOO_TILE(head_x - ix, head_y - iy).element;
				if (!(zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
					stat->step_x = 0;
					stat->step_y = 0;
				} else {
					stat->step_x = -ix;
					stat->step_y = -iy;
				}
			}
		}
	}

	int8_t head_stepx = stat->step_x;
	int8_t head_stepy = stat->step_y;

	if (head_stepx == 0 && head_stepy == 0) {
		// Invert centipede
		ZOO_TILE(head_x, head_y).element = E_CENTIPEDE_SEGMENT;
		stat->leader = STAT_ID_UNUSED;
		zoo_stat_t *stat_ptr = stat;
		while ((stat_ptr->follower > 0) && STAT_ID_VALID(stat_ptr->follower)) {
			tmp = stat_ptr->follower;
			stat_ptr->follower = stat_ptr->leader;
			stat_ptr->leader = tmp;

			stat_id = tmp;
			stat_ptr = &ZOO_STAT(stat_id);
		}
		stat_ptr->follower = stat_ptr->leader;
		stat_ptr->leader = STAT_ID_UNUSED;
		ZOO_TILE(stat_ptr->x, stat_ptr->y).element = E_CENTIPEDE_HEAD;
	} else if (ZOO_TILE(head_x + head_stepx, head_y + head_stepy).element == E_PLAYER) {
		// Attack player
		if (stat->follower > 0 && STAT_ID_VALID(stat->follower)) {
			zoo_stat_t *stat_follower = &ZOO_STAT(stat->follower);
			ZOO_TILE(stat_follower->x, stat_follower->y).element = E_CENTIPEDE_HEAD;
			stat_follower->step_x = head_stepx;
			stat_follower->step_y = head_stepy;
			board_draw_tile(stat_follower->x, stat_follower->y);
		}
		board_attack(stat_id, head_x + head_stepx, head_y + head_stepy);
	} else {
		// Move
		move_stat(stat_id, head_x + head_stepx, head_y + head_stepy);

		while (true) {
			int8_t ix = stat->step_x;
			int8_t iy = stat->step_y;
			uint8_t tx = stat->x - ix;
			uint8_t ty = stat->y - iy;

			if (!STAT_ID_VALID(stat->follower)) {
				if (ZOO_TILE(tx - ix, ty - iy).element == E_CENTIPEDE_SEGMENT) {
					uint8_t elem_stat_id = get_stat_id_at(tx - ix, ty - iy);
					if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
						stat->follower = elem_stat_id;
						goto NewFollowerFound;
					}
				}

				if (ZOO_TILE(tx - iy, ty - ix).element == E_CENTIPEDE_SEGMENT) {
					uint8_t elem_stat_id = get_stat_id_at(tx - iy, ty - ix);
					if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
						stat->follower = elem_stat_id;
						goto NewFollowerFound;
					}
				}

				if (ZOO_TILE(tx + iy, ty + ix).element == E_CENTIPEDE_SEGMENT) {
					uint8_t elem_stat_id = get_stat_id_at(tx + iy, ty + ix);
					if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
						stat->follower = elem_stat_id;
						// goto NewFollowerFound;
					}
				}
			}

NewFollowerFound:
			if ((stat->follower > 0) && STAT_ID_VALID(stat->follower)) {
				zoo_stat_t *stat_follower = &ZOO_STAT(stat->follower);
				stat_follower->leader = stat_id;
				stat_follower->p1 = stat->p1;
				stat_follower->p2 = stat->p2;
				stat_follower->step_x = tx - stat_follower->x;
				stat_follower->step_y = ty - stat_follower->y;
				move_stat(stat->follower, tx, ty);
			}
			
			stat_id = stat->follower;
			if (!STAT_ID_VALID(stat_id)) {
				break;
			}
			stat = &ZOO_STAT(stat_id);
		}
	}
}

void ElementCentipedeSegmentTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->leader == STAT_ID_UNUSED) {
		stat->leader = STAT_ID_UNUSED_STAGE2;
	} else if (stat->leader == STAT_ID_UNUSED_STAGE2) {
		ZOO_TILE(stat->x, stat->y).element = E_CENTIPEDE_HEAD;
	}
}

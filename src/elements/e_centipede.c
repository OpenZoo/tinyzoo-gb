#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

#define STAT_ID_VALID(id) ((id) < 254)

void ElementCentipedeHeadTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->x == ZOO_STAT(0).x && (rand(10) < stat->p1)) {
		stat->step_y = signum8(ZOO_STAT(0).y - stat->y);
		stat->step_x = 0;
	} else if (stat->y == ZOO_STAT(0).y && (rand(10) < stat->p1)) {
		stat->step_x = signum8(ZOO_STAT(0).x - stat->x);
		stat->step_x = 0;
	} else if ((rand(10 * 4) < stat->p2) || (stat->step_x == 0 && stat->step_y == 0)) {
		calc_direction_rnd(&stat->step_x, &stat->step_y);
	}
	
	uint8_t elem = ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element;
	if (!(zoo_element_defs[elem].flags & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
		uint8_t ix = stat->step_x;
		uint8_t iy = stat->step_y;
		uint8_t tmp = ((rand(2) << 1) - 1) * iy;
		stat->step_y = ((rand(2) << 1) - 1) * ix;
		stat->step_x = tmp;
		
		elem = ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element;
		if (!(zoo_element_defs[elem].flags & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
			elem = ZOO_TILE(stat->x - stat->step_x, stat->y - stat->step_y).element;
			if (!(zoo_element_defs[elem].flags & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
				elem = ZOO_TILE(stat->x - ix, stat->y - iy).element;
				if (!(zoo_element_defs[elem].flags & ELEMENT_WALKABLE) && (elem != E_PLAYER)) {
					stat->step_x = 0;
					stat->step_y = 0;
				} else {
					stat->step_x = -ix;
					stat->step_y = -iy;
				}
			} else {
				stat->step_x = -stat->step_x;
				stat->step_y = -stat->step_y;
			}
		}

		if (stat->step_x == 0 && stat->step_y == 0) {
			// Invert centipede
			ZOO_TILE(stat->x, stat->y).element = E_CENTIPEDE_SEGMENT;
			stat->leader = 255;
			zoo_stat_t *stat_ptr = &ZOO_STAT(stat_id);
			while ((stat->follower > 0) && STAT_ID_VALID(stat_ptr->follower)) {
				tmp = stat_ptr->follower;
				stat_ptr->follower = stat_ptr->leader;
				stat_ptr->leader = tmp;
				stat_id = tmp;
				stat_ptr = &ZOO_STAT(stat_id);
			}
			stat_ptr->follower = stat_ptr->leader;
			stat_ptr->leader = 255;
			ZOO_TILE(stat_ptr->x, stat_ptr->y).element = E_CENTIPEDE_HEAD;
		} else if (ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element == E_PLAYER) {
			// Attack player
			if ((stat->follower > 0) && STAT_ID_VALID(stat->follower)) {
				zoo_stat_t *stat_follower = &ZOO_STAT(stat->follower);
				stat_follower->step_x = stat->step_x;
				stat_follower->step_y = stat->step_y;
				ZOO_TILE(stat_follower->x, stat_follower->y).element = E_CENTIPEDE_HEAD;
				board_draw_tile(stat_follower->x, stat_follower->y);
			}
			board_attack(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
		} else {
			// Move
			move_stat(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);

			while (STAT_ID_VALID(stat_id)) {
				stat = &ZOO_STAT(stat_id);
				uint8_t ix = stat->step_x;
				uint8_t iy = stat->step_y;
				uint8_t tx = stat->x - ix;
				uint8_t ty = stat->y - iy;
				if (!STAT_ID_VALID(stat->follower)) {
					elem = ZOO_TILE(tx - ix, ty - iy).element;
					if (elem == E_CENTIPEDE_SEGMENT) {
						uint8_t elem_stat_id = get_stat_id_at(tx - ix, ty - iy);
						if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
							stat->follower = elem_stat_id;
							goto NewFollowerFound;
						}
					}

					elem = ZOO_TILE(tx - iy, ty - ix).element;
					if (elem == E_CENTIPEDE_SEGMENT) {
						uint8_t elem_stat_id = get_stat_id_at(tx - iy, ty - ix);
						if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
							stat->follower = elem_stat_id;
							goto NewFollowerFound;
						}
					}

					elem = ZOO_TILE(tx + iy, ty + ix).element;
					if (elem == E_CENTIPEDE_SEGMENT) {
						uint8_t elem_stat_id = get_stat_id_at(tx + iy, ty + ix);
						if (!STAT_ID_VALID(ZOO_STAT(elem_stat_id).leader)) {
							stat->follower = elem_stat_id;
							goto NewFollowerFound;
						}
					}
				}

NewFollowerFound:
				if ((stat->follower > 0) && STAT_ID_VALID(stat->follower)) {
					zoo_stat_t *stat_follower = &ZOO_STAT(stat->follower);
					stat_follower->p1 = stat->p1;
					stat_follower->p2 = stat->p2;
					stat_follower->leader = stat_id;
					stat_follower->step_x = tx - stat->x;
					stat_follower->step_y = tx - stat->y;
					move_stat(stat->follower, tx, ty);
				}
				stat_id = stat->follower;
			}
		}
	}
}

void ElementCentipedeSegmentTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->leader == 255) {
		stat->leader--;
	} else if (stat->leader == 254) {
		ZOO_TILE(stat->x, stat->y).element = E_CENTIPEDE_HEAD;
	}
}
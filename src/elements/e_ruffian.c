#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementRuffianTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->step_x == 0 && stat->step_y == 0) {
		if ((stat->p2 + 8) <= rand(17)) {
			if (stat->p1 >= rand(9)) {
				calc_direction_seek(stat->x, stat->y, &stat->step_x, &stat->step_y);
			} else {
				calc_direction_rnd(&stat->step_x, &stat->step_y);
			}
		}
	} else {
		uint8_t sx = stat->x;
		uint8_t sy = stat->y;

		if (((sx == ZOO_STAT(0).x) || (sy == ZOO_STAT(0).y)) && (rand(9) <= stat->p1)) {	
			calc_direction_seek(sx, sy, &stat->step_x, &stat->step_y);
		}

		uint8_t elem = ZOO_TILE(sx + stat->step_x, sy + stat->step_y).element;
		if (elem == E_PLAYER) {
			board_attack(stat_id, sx + stat->step_x, sy + stat->step_y);
		} else if (zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) {
			move_stat(stat_id, sx + stat->step_x, sy + stat->step_y);
			if ((stat->p2 + 8) <= rand(17)) {
				stat->step_x = 0;
				stat->step_y = 0;
			}
		} else {
			stat->step_x = 0;
			stat->step_y = 0;
		}
	}
}
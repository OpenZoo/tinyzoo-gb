#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementBulletTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	bool first_try = true;
	uint8_t ix, iy, ielem;
	
TryMove:
	ix = stat->x + stat->step_x;
	iy = stat->y + stat->step_y;
	ielem = ZOO_TILE(ix, iy).element;

	if ((zoo_element_defs[ielem].flags & ELEMENT_WALKABLE) || (ielem == E_WATER)) {
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
		(zoo_element_defs[ielem].flags & ELEMENT_DESTRUCTIBLE)
		&& ((ielem == E_PLAYER) || (stat->p1 == 0))
	)) {
		if (zoo_element_defs[ielem].score_value != 0) {
			zoo_world_info.score += zoo_element_defs[ielem].score_value;
			game_update_sidebar();
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
		// TODO: OopSend
	}
}
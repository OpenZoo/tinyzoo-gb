#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

uint8_t ElementPusherDraw(uint8_t x, uint8_t y) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->step_x == 1) {
		return 16;
	} else if (stat->step_x == -1) {
		return 17;
	} else if (stat->step_y == -1) {
		return 30;
	} else {
		return 31;
	}
}

void ElementPusherTick(uint8_t stat_id) {
	uint8_t i, start_x, start_y;
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	start_x = stat->x;
	start_y = stat->y;

	if (!(zoo_element_defs[ZOO_TILE(start_x + stat->step_x, start_y + stat->step_y).element].flags & ELEMENT_WALKABLE)) {
		ElementPushablePush(start_x + stat->step_x, start_y + stat->step_y, stat->step_x, stat->step_y);
	}
	
	stat_id = get_stat_id_at(start_x, start_y);
	stat = &ZOO_STAT(stat_id);

	if (zoo_element_defs[ZOO_TILE(start_x + stat->step_x, start_y + stat->step_y).element].flags & ELEMENT_WALKABLE) {
		move_stat(stat_id, start_x + stat->step_x, start_y + stat->step_y);
		sound_queue(2, sound_push);

		uint8_t nx = stat->x - (stat->step_x * 2);
		uint8_t ny = stat->y - (stat->step_y * 2);
		if (ZOO_TILE(nx, ny).element == E_PUSHER) {
			i = get_stat_id_at(nx, ny);
			zoo_stat_t *nstat = &ZOO_STAT(i);

			if (nstat->step_x == stat->step_x && nstat->step_y == stat->step_y) {
				ElementPusherTick(i);
			}
		}
	}
}

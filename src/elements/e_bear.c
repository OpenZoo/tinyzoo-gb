#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementBearTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	int8_t dx, dy;
	uint8_t elem;

	if (stat->x != ZOO_STAT(0).x) {
		if (difference8(stat->y, ZOO_STAT(0).y) <= (8 - stat->p1)) {
			dx = signum8(ZOO_STAT(0).x - stat->x);
			dy = 0;
			goto BearMovement;
		}
	}

	if (difference8(stat->x, ZOO_STAT(0).x) <= (8 - stat->p1)) {
		dx = 0;
		dy = signum8(ZOO_STAT(0).y - stat->y);
	} else {
		dx = 0;
		dy = 0;
	}

BearMovement:
	elem = ZOO_TILE(stat->x + dx, stat->y + dy).element;
	if (zoo_element_defs[elem].flags & ELEMENT_WALKABLE) {
		move_stat(stat_id, stat->x + dx, stat->y + dy);
	} else if (elem == E_PLAYER || elem == E_BREAKABLE) {
		board_attack(stat_id, stat->x + dx, stat->y + dy);
	}
}
#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementLionTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t sx = stat->x;
	uint8_t sy = stat->y;
	int8_t dx, dy;

	if (stat->p1 < rand(10)) {
		calc_direction_rnd(&dx, &dy);
	} else {
		calc_direction_seek(sx, sy, &dx, &dy);
	}

	uint8_t elem = ZOO_TILE(sx + dx, sy + dy).element;
	if (zoo_element_defs_flags[elem] & ELEMENT_WALKABLE) {
		move_stat(stat_id, sx + dx, sy + dy);
	} else if (elem == E_PLAYER) {
		board_attack(stat_id, sx + dx, sy + dy);
	}
}

void ElementTigerTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t element = (stat->p2 & 0x80) ? E_STAR : E_BULLET;

	if ((rand(10) * 3) < (stat->p2 & 0x7F)) {
		bool shot;

		if (difference8(stat->x, ZOO_STAT(0).x) <= 2) {
			shot = board_shoot(element, stat->x, stat->y,
				0, signum8(ZOO_STAT(0).y - stat->y),
				SHOT_SOURCE_ENEMY);
		} else {
			shot = false;
		}

		if (!shot) {
			if (difference8(stat->y, ZOO_STAT(0).y) <= 2) {
				shot = board_shoot(element, stat->x, stat->y,
					signum8(ZOO_STAT(0).x - stat->x), 0,
					SHOT_SOURCE_ENEMY);
			}
		}
	}

	ElementLionTick(stat_id);
}

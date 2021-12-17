#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementSharkTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	int8_t dx, dy;
	uint8_t sx = stat->x;
	uint8_t sy = stat->y;

	if (stat->p1 < rand(10)) {
		calc_direction_rnd(&dx, &dy);
	} else {
		calc_direction_seek(sx, sy, &dx, &dy);
	}

	switch (ZOO_TILE(sx + dx, sy + dy).element) {
		case E_WATER:
			move_stat(stat_id, sx + dx, sy + dy);
			break;
		case E_PLAYER:
			board_attack(stat_id, sx + dx, sy + dy);
			break;
	}
}
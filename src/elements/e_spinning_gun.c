#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t spinning_gun_tiles[4] = {24, 26, 25, 27};

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y) {
	return spinning_gun_tiles[(zoo_game_state.current_tick >> 1) & 3];
}

void ElementSpinningGunTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	board_draw_tile(stat->x, stat->y);

	int8_t dx, dy;
	uint8_t element = (stat->p2 & 0x80) ? E_STAR : E_BULLET;

	if (rand(9) < (stat->p2 & 0x7F)) {
		if (rand(9) <= stat->p1) {
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
		} else {
			calc_direction_rnd(&dx, &dy);
			board_shoot(element, stat->x, stat->y, dx, dy, SHOT_SOURCE_ENEMY);
		}
	}
}

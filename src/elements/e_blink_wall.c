#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementBlinkWallTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->p3 == 0) {
		stat->p3 = stat->p1 + 1;
	}

	if (stat->p3 == 1) {
		uint8_t ix = stat->x + stat->step_x;
		uint8_t iy = stat->y + stat->step_y;

		uint8_t elem = (stat->step_x != 0) ? E_BLINK_RAY_EW : E_BLINK_RAY_NS;
		uint8_t color = ZOO_TILE(stat->x, stat->y).color;

		// clear ray
		bool cleared_ray = false;
		while (true) {
			zoo_tile_t *tile = &ZOO_TILE(ix, iy);
			if (tile->element != elem || tile->color != color) {
				break;
			}
			tile->element = E_EMPTY;
			board_draw_tile(ix, iy);
			ix += stat->step_x;
			iy += stat->step_y;
			cleared_ray = true;
		}

		if (!cleared_ray) {
			// spawn new ray
			while (true) {
				zoo_tile_t *tile = &ZOO_TILE(ix, iy);
				if ((tile->element != E_EMPTY) && ((zoo_element_defs[tile->element].flags & ELEMENT_DESTRUCTIBLE))) {
					board_damage_tile(ix, iy);
				}

				if (tile->element == E_PLAYER) {
					uint8_t player_stat_id = get_stat_id_at(ix, iy);

					if (stat->step_x != 0) {
						if (ZOO_TILE(ix, iy - 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy - 1);
						} else if (ZOO_TILE(ix, iy + 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy + 1);
						}
					} else {
						if (ZOO_TILE(ix + 1, iy).element == E_EMPTY) {
							move_stat(player_stat_id, ix + 1, iy);
						} else if (ZOO_TILE(ix - 1, iy).element == E_EMPTY) {
							move_stat(player_stat_id, ix + 1, iy);
						}
					}

					if (tile->element == E_PLAYER) {
						while (zoo_world_info.health > 0)
							damage_stat(player_stat_id);
						goto HitBoundary;
					}
				}

				if (tile->element == E_EMPTY) {
					tile->element = elem;
					tile->color = color;
					board_draw_tile(ix, iy);
				} else {
					goto HitBoundary;
				}

				ix += stat->step_x;
				iy += stat->step_y;
			}
		}

HitBoundary:
		stat->p3 = stat->p2 * 2 + 1;
	} else {
		stat->p3--;
	}
}
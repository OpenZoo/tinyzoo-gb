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
		int8_t stepx = stat->step_x;
		int8_t stepy = stat->step_y;

		uint8_t ix = stat->x + stepx;
		uint8_t iy = stat->y + stepy;

		uint8_t elem = (stepx != 0) ? E_BLINK_RAY_EW : E_BLINK_RAY_NS;
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
			ix += stepx;
			iy += stepy;
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

					if (stepx != 0) {
						if (ZOO_TILE(ix, iy - 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy - 1);
						} else if (ZOO_TILE(ix, iy + 1).element == E_EMPTY) {
							move_stat(player_stat_id, ix, iy + 1);
						}
					} else {
						if (ZOO_TILE(ix + 1, iy).element == E_EMPTY) {
							move_stat(player_stat_id, ix + 1, iy);
						} else if (ZOO_TILE(ix - 1, iy).element == E_EMPTY) {
#ifdef BUGFIX_BLINKWALL_OFFSET
							move_stat(player_stat_id, ix - 1, iy);
#else
							move_stat(player_stat_id, ix + 1, iy);
#endif
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

				ix += stepx;
				iy += stepy;
			}
		}

HitBoundary:
		stat->p3 = stat->p2 * 2 + 1;
	} else {
		stat->p3--;
	}
}
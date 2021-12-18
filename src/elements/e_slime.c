#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementSlimeTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	if (stat->p1 < stat->p2) {
		stat->p1++;
	} else {
		stat->p1 = 0;
		uint8_t start_x = stat->x;
		uint8_t start_y = stat->y;
		uint8_t color_copy = ZOO_TILE(start_x, start_y).color;
		bool changed_tiles = false;

		for (uint8_t dir = 0; dir < 4; dir++) {
			uint8_t tx = start_x + neighbor_delta_x[dir];
			uint8_t ty = start_y + neighbor_delta_y[dir];

			if (zoo_element_defs_flags[ZOO_TILE(tx, ty).element] & ELEMENT_WALKABLE) {
				if (!changed_tiles) {
					changed_tiles = true;
					move_stat(stat_id, tx, ty);
					ZOO_TILE_CHANGE2(start_x, start_y, E_BREAKABLE, color_copy);
					board_draw_tile(start_x, start_y);
				} else {
					add_stat(tx, ty, E_SLIME, color_copy, 3, &stat_template_default);
					ZOO_STAT(zoo_stat_count).p2 = stat->p2;
				}
			}
		}

		if (!changed_tiles) {
			remove_stat(stat_id);
			ZOO_TILE_CHANGE2(start_x, start_y, E_BREAKABLE, color_copy);
			board_draw_tile(start_x, start_y);
		}
	}
}

void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t color_copy = tile->color;
	damage_stat(get_stat_id_at(x, y));
	tile->element = E_BREAKABLE;
	tile->color = color_copy;

	board_draw_tile(x, y);
	sound_queue(2, sound_slime_touch);
}

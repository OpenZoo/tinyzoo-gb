#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t star_tiles[4] = {179, '/', 196, '\\'};

uint8_t ElementStarDraw(uint8_t x, uint8_t y) {
	uint8_t c = ZOO_TILE(x, y).color;
	if (c >= 15) { c = 9; } else { c++; }
	ZOO_TILE(x, y).color = c;
	return star_tiles[zoo_game_state.current_tick & 3];
}

void ElementStarTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t p2 = --stat->p2;
	if (p2 <= 0) {
		remove_stat(stat_id);
	} else {
		uint8_t sx = stat->x;
		uint8_t sy = stat->y;
		if (!(p2 & 1)) {
			calc_direction_seek(sx, sy, &stat->step_x, &stat->step_y);
			uint8_t* t_elem_ptr = &(ZOO_TILE(sx + stat->step_x, sy + stat->step_y).element);
			uint8_t t_elem = *t_elem_ptr;
			if (t_elem == E_PLAYER || t_elem == E_BREAKABLE) {
				board_attack(stat_id, sx + stat->step_x, sy + stat->step_y);
			} else {
				if (!(zoo_element_defs[t_elem].flags & ELEMENT_WALKABLE)) {
					ElementPushablePush(sx + stat->step_x, sy + stat->step_y, stat->step_x, stat->step_y);
					t_elem = *t_elem_ptr;
				}

				if ((zoo_element_defs[t_elem].flags & ELEMENT_WALKABLE) || (t_elem == E_WATER)) {
					move_stat(stat_id, sx + stat->step_x, sy + stat->step_y);
				}
			}
		} else {
			board_draw_tile(sx, sy);
		}
	}
}
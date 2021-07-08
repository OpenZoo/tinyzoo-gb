#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t duplicator_tiles[6] = {250, 250, 249, 248, 111, 79};

uint8_t ElementDuplicatorDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 >= 6) p1 = 0;
	return duplicator_tiles[p1];
}

void ElementDuplicatorTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	if (stat->p1 <= 4) {
		stat->p1++;
	} else {
		stat->p1 = 0;
		uint8_t src_elem = ZOO_TILE(stat->x - stat->step_x, stat->y - stat->step_y).element;
		if (src_elem == E_PLAYER) {
			uint8_t dst_elem = ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element;
			zoo_element_defs[dst_elem].touch_proc(stat->x + stat->step_x, stat->y + stat->step_y, &input_delta_x, &input_delta_y);
		} else {
			if (src_elem != E_EMPTY) {
				ElementPushablePush(stat->x - stat->step_x, stat->y - stat->step_y, -stat->step_x, -stat->step_y);
 				src_elem = ZOO_TILE(stat->x - stat->step_x, stat->y - stat->step_y).element;
 			}

			if (src_elem == E_EMPTY) {
				uint8_t source_stat_id = get_stat_id_at(stat->x + stat->step_x, stat->y + stat->step_y);
				if (source_stat_id == 255) {
					ZOO_TILE_COPY(
						ZOO_TILE(stat->x - stat->step_x, stat->y - stat->step_y),
						ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y)
					);
				} else if (source_stat_id != 0) {
					zoo_tile_t src_tile;
					ZOO_TILE_ASSIGN(src_tile, stat->x + stat->step_x, stat->y + stat->step_y);
					zoo_stat_t *src_stat = &ZOO_STAT(source_stat_id);

					add_stat(stat->x - stat->step_x, stat->y - stat->step_y,
						src_tile.element, src_tile.color,
						src_stat->cycle, src_stat);
				} else {
					goto PostPlace;
				}
				board_draw_tile(stat->x - stat->step_x, stat->y - stat->step_y);

PostPlace:
				sound_queue(3, sound_duplicator_success);
			} else {
				sound_queue(3, sound_duplicator_failure);
			}
		}
		stat->p1 = 0;
	}
	
	board_draw_tile(stat->x, stat->y);
	stat->cycle = (9 - stat->p2) * 3;
}
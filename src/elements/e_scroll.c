#pragma bank 1

#include <stdbool.h>
#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../oop.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementScrollTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (!ZOO_TILE_WRITEBOUNDS(stat->x, stat->y)) return;

	zoo_tile_t *tile = &ZOO_TILE(stat->x, stat->y);
	if (tile->color >= 15) {
		tile->color = 9;
	} else {
		tile->color++;
	}

	board_draw_tile(stat->x, stat->y);
}

void ElementScrollTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	uint8_t stat_id = get_stat_id_at(x, y);
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	sound_queue(2, sound_scroll_touch);

	stat->data_pos = 0;
	if (oop_execute(stat_id, oop_scroll_name)) {
#ifdef BUGFIX_DIEMOVE_SCROLL
		return;
#endif
	}

	stat_id = get_stat_id_at(x, y);
	if (stat_id != STAT_ID_NONE) {
		remove_stat(stat_id);
	}
}

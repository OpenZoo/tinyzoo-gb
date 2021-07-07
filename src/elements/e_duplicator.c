#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static uint8_t duplicator_tiles[6] = {250, 250, 249, 248, 111, 79};

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
		// TODO
	}
	
	board_draw_tile(stat->x, stat->y);
	stat->cycle = (9 - stat->p2) * 3;
}
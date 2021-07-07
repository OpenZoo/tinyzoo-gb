#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

uint8_t ElementBombDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 <= 1) return 11;
	else return 48 + p1;
}

void ElementBombTick(uint8_t stat_id) {
	// TODO
}

void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->p1 == 0) {
		stat->p1 = 9;
		board_draw_tile(x, y);
		// TODO
		sound_queue(4, sound_bomb_activated);
	} else {
		ElementPushablePush(x, y, *dx, *dy);
	}
}
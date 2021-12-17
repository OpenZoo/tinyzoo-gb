#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

uint8_t ElementBombDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 <= 1) return 11;
	else return 48 + p1;
}

void ElementBombTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->p1 > 0) {
		uint8_t p1 = --stat->p1;
		board_draw_tile(stat->x, stat->y);

		if (p1 == 1) {
			sound_queue(1, sound_bomb_explosion);
			DrawPlayerSurroundings(stat->x, stat->y, 1);
		} else if (p1 == 0) {
			uint8_t old_x = stat->x;
			uint8_t old_y = stat->y;
			remove_stat(stat_id);
			DrawPlayerSurroundings(old_x, old_y, 2);
		} else if ((p1 & 1) == 0) {
			sound_queue(1, sound_bomb_tick1);
		} else {
			sound_queue(1, sound_bomb_tick2);
		}
	}
}

void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->p1 == 0) {
		stat->p1 = 9;
		board_draw_tile(x, y);
		display_message(200, NULL, NULL, msg_bomb_activated);
		sound_queue(4, sound_bomb_activated);
	} else {
		ElementPushablePush(x, y, *dx, *dy);
	}
}
#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementCentipedeHeadTick(uint8_t stat_id) {
	// TODO
}

void ElementCentipedeSegmentTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->leader == 255) {
		stat->leader--;
	} else if (stat->leader == 254) {
		ZOO_TILE(stat->x, stat->y).element = E_CENTIPEDE_HEAD;
	}
}
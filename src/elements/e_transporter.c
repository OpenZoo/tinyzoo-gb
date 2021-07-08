#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t transporter_ns_tiles[8] = {'^', '~', '^', '-', 'v', '_', 'v', '-'};
static const uint8_t transporter_ew_tiles[8] = {'(', '<', '(', 179, ')', '>', ')', 179};

uint8_t ElementTransporterDraw(uint8_t x, uint8_t y) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	uint8_t dvc;
	if (stat->cycle > 0) {
		dvc = (zoo_game_state.current_tick / stat->cycle) & 3;
	} else {
		dvc = 0;
	}
	if (stat->step_x == 0) {
		return transporter_ns_tiles[stat->step_y * 2 + 2 + dvc];
	} else {
		return transporter_ew_tiles[stat->step_x * 2 + 2 + dvc];
	}
}

void ElementTransporterTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	board_draw_tile(stat->x, stat->y);
}

void ElementTransporterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ElementTransporterMove(x - *dx, y - *dy, *dx, *dy);
	*dx = 0;
	*dy = 0;
}
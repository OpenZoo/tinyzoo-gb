#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static void ElementConveyorTick(uint8_t x, uint8_t y, int8_t dir) {
	uint8_t i_min, i_max;
	if (dir == 1) {
		i_min = 0;
		i_max = 7;
	} else {
		i_min = 7;
		i_max = 0;
	}

	bool can_move = true;
	uint8_t tx, ty;
	zoo_tile_t tiles[8];
	uint8_t stat_ids[8];

	for (uint8_t i = i_min; i != i_max; i += dir) {
		tx = x + diagonal_delta_x[i];
		ty = y + diagonal_delta_y[i];
		zoo_tile_t ttile;
		ZOO_TILE_COPY(ttile, ZOO_TILE(tx, ty));
		ZOO_TILE_COPY(tiles[i], ttile);
		if (ttile.element == E_EMPTY) {
			can_move = true;
		} else if (!(zoo_element_defs[ttile.element].flags & ELEMENT_PUSHABLE)) {
			can_move = false;
		}

		if (zoo_element_defs[ttile.element].flags & ELEMENT_TYPICALLY_STATTED) {
			stat_ids[i] = get_stat_id_at(tx, ty);
		}
	}

	for (uint8_t i = i_min; i != i_max; i += dir) {
		zoo_tile_t ttile;
		ZOO_TILE_COPY(ttile, tiles[i]);

		if (can_move) {
			if (zoo_element_defs[ttile.element].flags & ELEMENT_PUSHABLE) {
				uint8_t ix = x + diagonal_delta_x[(i - dir) & 7];
				uint8_t iy = y + diagonal_delta_y[(i - dir) & 7];
				tx = x + diagonal_delta_x[i];
				ty = y + diagonal_delta_y[i];

				if (zoo_element_defs[ttile.element].flags & ELEMENT_TYPICALLY_STATTED) {
					zoo_tile_t tmp_tile;
					ZOO_TILE_COPY(tmp_tile, ZOO_TILE(tx, ty));
					ZOO_TILE_COPY(ZOO_TILE(tx, ty), ttile);
					ZOO_TILE(ix, iy).element = E_EMPTY;
					move_stat(stat_ids[i], ix, iy);

					ZOO_TILE_COPY(ZOO_TILE(tx, ty), tmp_tile);
					board_draw_tile(tx, ty);
				} else {
					ZOO_TILE_COPY(ZOO_TILE(ix, iy), ttile);					
					board_draw_tile(ix, iy);
				}

				if (!(zoo_element_defs[tiles[(i + dir) & 7].element].flags & ELEMENT_PUSHABLE)) {
					ZOO_TILE(tx, ty).element = E_EMPTY;
					board_draw_tile(tx, ty);
				}
			} else {
				can_move = false;
			}
		} else if (ttile.element == E_EMPTY) {
			can_move = true;
		} else if (!(zoo_element_defs[ttile.element].flags & ELEMENT_PUSHABLE)) {
			can_move = false;
		}
	}
}

static uint8_t conveyor_cw_tiles[4] = {179, 47, 196, 92};

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick / 3) & 3)];
}

void ElementConveyorCWTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	board_draw_tile(stat->x, stat->y);
	ElementConveyorTick(stat->x, stat->y, -1);
}

uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick >> 1) & 3) ^ 3];
}

void ElementConveyorCCWTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	board_draw_tile(stat->x, stat->y);
	ElementConveyorTick(stat->x, stat->y, 1);
}

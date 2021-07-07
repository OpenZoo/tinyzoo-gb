#include "elements.h"
#include "gamevars.h"
#include "game.h"
#include "input.h"
#include "math.h"
#include "sound_consts.h"
#include "timer.h"

void ElementMove(uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y) {
	uint8_t stat_id = get_stat_id_at(old_x, old_y);
	if (stat_id != STAT_ID_NONE) {
		move_stat(stat_id, new_x, new_y);
	} else {
		ZOO_TILE_COPY(ZOO_TILE(new_x, new_y), ZOO_TILE(old_x, old_y));
		board_draw_tile(new_x, new_y);
		ZOO_TILE(old_x, old_y).element = E_EMPTY;
		board_draw_tile(old_x, old_y);
	}
}

void ElementTransporterMove(uint8_t x, uint8_t y, int8_t dx, int8_t dy);

void ElementPushablePush(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	ZOO_TILE_ASSIGN(tile, x, y);
	if (((tile.element == E_SLIDER_NS) && (dx == 0)) || ((tile.element == E_SLIDER_EW) && (dy == 0))
		|| (zoo_element_defs[tile.element].flags & ELEMENT_PUSHABLE)) {
		
		zoo_tile_t dtile;
		ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		if (dtile.element == E_TRANSPORTER) {
			ElementTransporterMove(x, y, dx, dy);
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		} else if (dtile.element != E_EMPTY) {
			if (dx != 0 || dy != 0) {
				ElementPushablePush(x + dx, y + dy, dx, dy);
				ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
			}
		}

		if (!(zoo_element_defs[dtile.element].flags & ELEMENT_WALKABLE)
			&& (zoo_element_defs[dtile.element].flags & ELEMENT_DESTRUCTIBLE)
			&& (tile.element != E_PLAYER))
		{
			// TODO BoardDamageTile
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		}

		if (zoo_element_defs[dtile.element].flags & ELEMENT_WALKABLE) {
			ElementMove(x, y, x + dx, y + dy);
		}
	}
}

void ElementDefaultTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {

}

void ElementDamagingTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementBoardEdgeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementMessageTimerTick(uint8_t stat_id) {
	// TODO
}

void ElementMonitorTick(uint8_t stat_id) {
	// TODO
}

static int8_t player_dir_x, player_dir_y;

void ElementPlayerTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	// TODO: energizer ticks

	// TODO: health <= 0

	// TODO: shoot
	if (input_delta_x != 0 || input_delta_y != 0) {
		player_dir_x = input_delta_x;
		player_dir_y = input_delta_y;

		zoo_element_defs[ZOO_TILE(stat->x + input_delta_x, stat->y + input_delta_y).element]
			.touch_proc(stat->x + input_delta_x, stat->y + input_delta_y, &input_delta_x, &input_delta_y);

		if (input_delta_x != 0 || input_delta_y != 0) {
			if (zoo_element_defs[ZOO_TILE(stat->x + input_delta_x, stat->y + input_delta_y).element].flags & ELEMENT_WALKABLE) {
				move_stat(0, stat->x + input_delta_x, stat->y + input_delta_y);
			}
		}
	}
	
	// TODO
}

void ElementAmmoTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.ammo += 5;

	ZOO_TILE(x, y).element = E_EMPTY;
	// TODO
}

void ElementTorchTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.torches += 1;
	ZOO_TILE(x, y).element = E_EMPTY;
	
	board_draw_tile(x, y);
	// TODO
}

void ElementGemTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.gems += 1;
	zoo_world_info.health += 1;
	zoo_world_info.score += 10;

	ZOO_TILE(x, y).element = E_EMPTY;
	// TODO
}

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementScrollTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	zoo_tile_t *tile = &ZOO_TILE(stat->x, stat->y);
	if (tile->color >= 15) {
		tile->color = 9;
	} else {
		tile->color++;
	}

	board_draw_tile(stat->x, stat->y);
}

void ElementScrollTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementPassageTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

static uint8_t duplicator_tiles[6] = {250, 250, 249, 248, 111, 79};

uint8_t ElementDuplicatorDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 >= 6) p1 = 0;
	return duplicator_tiles[p1];
}

void ElementDuplicatorTick(uint8_t stat_id) {
	// TODO
}

uint8_t ElementBombDraw(uint8_t x, uint8_t y) {
	uint8_t p1 = ZOO_STAT_AT(x, y).p1;
	if (p1 <= 1) return 11;
	else return 48 + p1;
}

void ElementBombTick(uint8_t stat_id) {
	// TODO
}

void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementEnergizerTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

static uint8_t star_tiles[4] = {179, '/', 196, '\\'};

uint8_t ElementStarDraw(uint8_t x, uint8_t y) {
	uint8_t c = ZOO_TILE(x, y).color;
	if (c >= 15) { c = 9; } else { c++; }
	ZOO_TILE(x, y).color = c;
	return star_tiles[zoo_game_state.current_tick & 3];
}

void ElementStarTick(uint8_t stat_id) {
	// TODO
}

static uint8_t conveyor_cw_tiles[4] = {179, 47, 196, 92};

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick / 3) & 3)];
}

void ElementConveyorCWTick(uint8_t stat_id) {
	// TODO
}

uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y) {
	return conveyor_cw_tiles[((zoo_game_state.current_tick >> 1) & 3) ^ 3];
}

void ElementConveyorCCWTick(uint8_t stat_id) {
	// TODO
}

void ElementBulletTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	bool first_try = true;
	uint8_t ix, iy, ielem;
	
TryMove:
	ix = stat->x + stat->step_x;
	iy = stat->y + stat->step_y;
	ielem = ZOO_TILE(ix, iy).element;

	if ((zoo_element_defs[ielem].flags & ELEMENT_WALKABLE) || (ielem == E_WATER)) {
		move_stat(stat_id, ix, iy);
		return;
	}

	if ((ielem == E_RICOCHET) && first_try) {
		stat->step_x = -(stat->step_x);
		stat->step_y = -(stat->step_y);
		sound_queue(1, sound_ricochet);
		first_try = false;
		goto TryMove;
	}

	if ((ielem == E_BREAKABLE) || (
		(zoo_element_defs[ielem].flags & ELEMENT_DESTRUCTIBLE)
		&& ((ielem == E_PLAYER) || (stat->p1 == 0))
	)) {
		if (zoo_element_defs[ielem].score_value != 0) {
			zoo_world_info.score += zoo_element_defs[ielem].score_value;
			// TODO: GameUpdateSidebar
		}
		// TODO: BoardAttack
		// return;
	}

	if (first_try) {
		uint8_t relem = ZOO_TILE(stat->x + stat->step_y, stat->y + stat->step_x).element;
		if (relem == E_RICOCHET) {
			ix = stat->step_x;
			stat->step_x = -stat->step_y;
			stat->step_y = -ix;
			sound_queue(1, sound_ricochet);
			first_try = false;
			goto TryMove;
		}

		relem = ZOO_TILE(stat->x - stat->step_y, stat->y - stat->step_x).element;
		if (relem == E_RICOCHET) {
			ix = stat->step_x;
			stat->step_x = stat->step_y;
			stat->step_y = ix;
			sound_queue(1, sound_ricochet);
			first_try = false;
			goto TryMove;
		}
	}

	remove_stat(stat_id);
	zoo_game_state.current_stat_ticked--;
	if ((ielem == E_OBJECT) || (ielem == E_SCROLL)) {
		// TODO: OopSend
	}
}

void ElementWaterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementForestTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementPushableTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ElementPushablePush(x, y, *dx, *dy);
	sound_queue(2, sound_push);
}

void ElementFakeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementInvisibleTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ZOO_TILE(x, y).element = E_NORMAL;
	board_draw_tile(x, y);

	// TODO
}

void ElementBlinkWallTick(uint8_t stat_id) {
	// TODO
}

static uint8_t transporter_ns_tiles[8] = {'^', '~', '^', '-', 'v', '_', 'v', '-'};
static uint8_t transporter_ew_tiles[8] = {'(', '<', '(', 179, ')', '>', ')', 179};

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

void ElementTransporterMove(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	zoo_stat_t *stat = &ZOO_STAT_AT(x + dx, y + dy);
	if ((stat->step_x == dx) && (stat->step_y == dy)) {
		uint8_t ix = x;
		uint8_t iy = y;
		bool is_valid_dest = true;
		while (1) {
			ix += dx;
			iy += dy;
			ZOO_TILE_ASSIGN(tile, ix, iy);
			if (tile.element == E_BOARD_EDGE) {
				break;
			} else if (is_valid_dest) {
				is_valid_dest = false;

				if (!(zoo_element_defs[tile.element].flags & ELEMENT_WALKABLE)) {
					ElementPushablePush(ix, iy, dx, dy);
					ZOO_TILE_ASSIGN(tile, ix, iy);
				}

				if (zoo_element_defs[tile.element].flags & ELEMENT_WALKABLE) {
					ElementMove(stat->x - dx, stat->y - dy, ix, iy);
					sound_queue(3, sound_transporter_move);
					break;
				}
			}

			if (tile.element == E_TRANSPORTER) {
				zoo_stat_t *stat2 = &ZOO_STAT_AT(ix, iy);
				if (stat2->step_x == -dx && stat2->step_y == -dy) {
					is_valid_dest = true;
				}
			}
		}
	}
}

void ElementTransporterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ElementTransporterMove(x - *dx, y - *dy, *dx, *dy);
	*dx = 0;
	*dy = 0;
}

uint8_t ElementLineDraw(uint8_t x, uint8_t y) {
	uint8_t i, v, shift;
	v = 0;
	shift = 1;
	// could be unrolled...
	for (i = 0; i < 4; i++, shift <<= 1) {
		uint8_t element = ZOO_TILE(x + neighbor_delta_x[i], y + neighbor_delta_y[i]).element;
		if (element == E_LINE || element == E_BOARD_EDGE) {
			v |= shift;
		}
	}
	return line_tiles[v];
}

void ElementBearTick(uint8_t stat_id) {
	// TODO
}

void ElementRuffianTick(uint8_t stat_id) {
	// TODO
}

void ElementSlimeTick(uint8_t stat_id) {
	// TODO
}

void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementSharkTick(uint8_t stat_id) {
	// TODO
}

static uint8_t spinning_gun_tiles[4] = {24, 26, 25, 27};

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y) {
	return spinning_gun_tiles[(zoo_game_state.current_tick >> 1) & 3];
}

void ElementSpinningGunTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	board_draw_tile(stat->x, stat->y);

	uint8_t dx, dy;
	uint8_t element = (stat->p2 & 0x80) ? E_STAR : E_BULLET;

	if (rand(9) < (stat->p2 & 0x7F)) {
		if (rand(9) <= stat->p1) {
			bool shot;

			if (difference8(stat->x, ZOO_STAT(0).x) <= 2) {
				shot = board_shoot(element, stat->x, stat->y,
					0, signum8(ZOO_STAT(0).y - stat->y),
					SHOT_SOURCE_ENEMY);
			} else {
				shot = false;
			}

			if (!shot) {
				if (difference8(stat->y, ZOO_STAT(0).y) <= 2) {
					shot = board_shoot(element, stat->x, stat->y,
						signum8(ZOO_STAT(0).x - stat->x), 0,
						SHOT_SOURCE_ENEMY);
				}
			}
		} else {
			calc_direction_rnd(&dx, &dy);
			board_shoot(element, stat->x, stat->y, dx, dy, SHOT_SOURCE_ENEMY);
		}
	}
}

uint8_t ElementPusherDraw(uint8_t x, uint8_t y) {
	zoo_stat_t *stat = &ZOO_STAT_AT(x, y);
	if (stat->step_x == 1) {
		return 16;
	} else if (stat->step_x == -1) {
		return 17;
	} else if (stat->step_y == -1) {
		return 30;
	} else {
		return 31;
	}
}

void ElementPusherTick(uint8_t stat_id) {
	// TODO
}

void ElementLionTick(uint8_t stat_id) {
	// TODO
}

void ElementTigerTick(uint8_t stat_id) {
	// TODO
}

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

uint8_t ElementObjectDraw(uint8_t x, uint8_t y) {
	uint8_t stat_id = get_stat_id_at(x, y);
	return ZOO_STAT(stat_id).p1;
}

void ElementObjectTick(uint8_t stat_id) {
	// TODO
}

void ElementObjectTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

#include "element_defs.inc"
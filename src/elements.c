#include "elements.h"
#include "elements_utils.h"
#include "gamevars.h"
#include "game.h"
#include "input.h"
#include "math.h"
#include "sound_consts.h"
#include "timer.h"

void ElementDefaultTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {

}

void ElementDamagingTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	board_attack(0, x, y);
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
	zoo_tile_t *tile = &ZOO_TILE(stat->x, stat->y);

	if (zoo_world_info.energizer_ticks > 0) {
		// TODO: player char

		if ((zoo_game_state.current_tick & 1) != 0) {
			tile->color = 0x0F;
		} else {
			tile->color = ((zoo_game_state.current_tick % 7) << 4) + 0x1F;
		}

		board_draw_tile(stat->x, stat->y);
	} else if (tile->color != 0x1F) {
		tile->color = 0x1F;
		board_draw_tile(stat->x, stat->y);
	}

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
	game_update_sidebar();
	sound_queue(2, sound_ammo_pickup);

	// TODO
}

void ElementTorchTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.torches += 1;
	ZOO_TILE(x, y).element = E_EMPTY;
	
	board_draw_tile(x, y);
	game_update_sidebar();

	// TODO

	sound_queue(3, sound_torch_pickup);
}

void ElementGemTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.gems += 1;
	zoo_world_info.health += 1;
	zoo_world_info.score += 10;

	ZOO_TILE(x, y).element = E_EMPTY;
	game_update_sidebar();
	sound_queue(2, sound_gem_pickup);

	// TODO
}

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color & 7);
	if (key == 0) {
		// TODO: black key
	} else {
		uint8_t key_shift = 1 << key;

		if (zoo_world_info.keys & key_shift) {
			// TODO
			sound_queue(2, sound_key_failure);
		} else {
			zoo_world_info.keys |= key_shift;
			tile->element = E_EMPTY;
			game_update_sidebar();

			// TODO
			sound_queue(2, sound_key_success);
		}
	}
}

void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color >> 4) & 7;
	if (key == 0) {
		// TODO: black door
	} else {
		uint8_t key_shift = 1 << key;

		if (!(zoo_world_info.keys & key_shift)) {
			// TODO
			sound_queue(3, sound_door_failure);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.keys &= ~key_shift;
			game_update_sidebar();

			// TODO
			sound_queue(3, sound_door_success);
		}
	}
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

void ElementEnergizerTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO

	ZOO_TILE(x, y).element = E_EMPTY;
	board_draw_tile(x, y);

	zoo_world_info.energizer_ticks = 75;
	game_update_sidebar();

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
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	stat->p2--;
	if (stat->p2 <= 0) {
		remove_stat(stat_id);
	} else if (!(stat->p2 & 1)) {
		calc_direction_seek(stat->x, stat->y, &stat->step_x, &stat->step_y);
		uint8_t* t_elem_ptr = &(ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element);
		uint8_t t_elem = *t_elem_ptr;
		if (t_elem == E_PLAYER || t_elem == E_BREAKABLE) {
			board_attack(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
		} else {
			if (!(zoo_element_defs[t_elem].flags & ELEMENT_WALKABLE)) {
				ElementPushablePush(stat->x + stat->step_x, stat->y + stat->step_y, stat->step_x, stat->step_y);
				t_elem = *t_elem_ptr;
			}

			if ((zoo_element_defs[t_elem].flags & ELEMENT_WALKABLE) || (t_elem == E_WATER)) {
				move_stat(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
			}
		}
	} else {
		board_draw_tile(stat->x, stat->y);
	}
}

void ElementConveyorTick(uint8_t x, uint8_t y, int8_t dir) {
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
			game_update_sidebar();
		}
		board_attack(stat_id, ix, iy);
		return;
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
	sound_queue(3, sound_water_splash);
	// TODO
}

void ElementForestTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ZOO_TILE(x, y).element = E_EMPTY;
	board_draw_tile(x, y);

	sound_queue(3, sound_forest);

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

	sound_queue(3, sound_invisible);

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
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	int8_t dx, dy;
	uint8_t elem;

	if (stat->x != ZOO_STAT(0).x) {
		if (difference8(stat->y, ZOO_STAT(0).y) <= (8 - stat->p1)) {
			dx = signum8(ZOO_STAT(0).x - stat->x);
			dy = 0;
			goto BearMovement;
		}
	}

	if (difference8(stat->x, ZOO_STAT(0).x) <= (8 - stat->p1)) {
		dx = 0;
		dy = signum8(ZOO_STAT(0).y - stat->y);
	} else {
		dx = 0;
		dy = 0;
	}

BearMovement:
	elem = ZOO_TILE(stat->x + dx, stat->y + dy).element;
	if (zoo_element_defs[elem].flags & ELEMENT_WALKABLE) {
		move_stat(stat_id, stat->x + dx, stat->y + dy);
	} else if (elem == E_PLAYER || elem == E_BREAKABLE) {
		board_attack(stat_id, stat->x + dx, stat->y + dy);
	}
}

void ElementRuffianTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->step_x == 0 && stat->step_y == 0) {
		if ((stat->p2 + 8) <= rand(17)) {
			if (stat->p1 >= rand(9)) {
				calc_direction_seek(stat->x, stat->y, &stat->step_x, &stat->step_y);
			} else {
				calc_direction_rnd(&stat->step_x, &stat->step_y);
			}
		}
	} else {
		if (((stat->x == ZOO_STAT(0).x) || (stat->y == ZOO_STAT(0).y)) && (rand(9) <= stat->p1)) {	
			calc_direction_seek(stat->x, stat->y, &stat->step_x, &stat->step_y);
		}

		uint8_t elem = ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element;
		if (elem == E_PLAYER) {
			board_attack(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
		} else if (zoo_element_defs[elem].flags & ELEMENT_WALKABLE) {
			move_stat(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
			if ((stat->p2 + 8) <= rand(17)) {
				stat->step_x = 0;
				stat->step_y = 0;
			}
		} else {
			stat->step_x = 0;
			stat->step_y = 0;
		}
	}
}

void ElementSlimeTick(uint8_t stat_id) {
	// TODO
}

void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	// TODO
}

void ElementSharkTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	int8_t dx, dy;

	if (stat->p1 < rand(10)) {
		calc_direction_rnd(&dx, &dy);
	} else {
		calc_direction_seek(stat->x, stat->y, &dx, &dy);
	}

	switch (ZOO_TILE(stat->x + dx, stat->y + dy).element) {
		case E_WATER:
			move_stat(stat_id, stat->x + dx, stat->y + dy);
			break;
		case E_PLAYER:
			board_attack(stat_id, stat->x + dx, stat->y + dy);
			break;
	}
}

static uint8_t spinning_gun_tiles[4] = {24, 26, 25, 27};

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y) {
	return spinning_gun_tiles[(zoo_game_state.current_tick >> 1) & 3];
}

void ElementSpinningGunTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	board_draw_tile(stat->x, stat->y);

	int8_t dx, dy;
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
	uint8_t i, start_x, start_y;
	zoo_stat_t *stat = &ZOO_STAT(stat_id);

	start_x = stat->x;
	start_y = stat->y;

	if (!(zoo_element_defs[ZOO_TILE(start_x + stat->step_x, start_y + stat->step_y).element].flags & ELEMENT_WALKABLE)) {
		ElementPushablePush(start_x + stat->step_x, start_y + stat->step_y, stat->step_x, stat->step_y);
	}
	
	stat_id = get_stat_id_at(start_x, start_y);
	stat = &ZOO_STAT(stat_id);
	if (zoo_element_defs[ZOO_TILE(stat->x + stat->step_x, stat->y + stat->step_y).element].flags & ELEMENT_WALKABLE) {
		move_stat(stat_id, stat->x + stat->step_x, stat->y + stat->step_y);
		sound_queue(2, sound_push);

		uint8_t nx = stat->x - (stat->step_x * 2);
		uint8_t ny = stat->y - (stat->step_y * 2);
		if (ZOO_TILE(nx, ny).element == E_PUSHER) {
			i = get_stat_id_at(nx, ny);
			zoo_stat_t *nstat = &ZOO_STAT(i);

			if (nstat->step_x == stat->step_x && nstat->step_y == stat->step_y) {
				ElementPusherTick(i);
			}
		}
	}
}

void ElementLionTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	int8_t dx, dy;

	if (stat->p1 < rand(10)) {
		calc_direction_rnd(&dx, &dy);
	} else {
		calc_direction_seek(stat->x, stat->y, &dx, &dy);
	}

	uint8_t elem = ZOO_TILE(stat->x + dx, stat->y + dy).element;
	if (zoo_element_defs[elem].flags & ELEMENT_WALKABLE) {
		move_stat(stat_id, stat->x + dx, stat->y + dy);
	} else if (elem == E_PLAYER) {
		board_attack(stat_id, stat->x + dx, stat->y + dy);
	}
}

void ElementTigerTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t element = (stat->p2 & 0x80) ? E_STAR : E_BULLET;

	if ((rand(10) * 3) < (stat->p2 & 0x7F)) {
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
	}

	ElementLionTick(stat_id);
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
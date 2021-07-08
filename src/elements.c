#pragma bank 1

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

void ElementPlayerTick(uint8_t stat_id);

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

uint8_t ElementDuplicatorDraw(uint8_t x, uint8_t y);
void ElementDuplicatorTick(uint8_t stat_id);

uint8_t ElementBombDraw(uint8_t x, uint8_t y);
void ElementBombTick(uint8_t stat_id);
void ElementBombTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementEnergizerTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	sound_queue(9, sound_energizer_loop);

	ZOO_TILE(x, y).element = E_EMPTY;
	board_draw_tile(x, y);

	zoo_world_info.energizer_ticks = 75;
	game_update_sidebar();

	// TODO
}

uint8_t ElementStarDraw(uint8_t x, uint8_t y);
void ElementStarTick(uint8_t stat_id);

void ElementBulletTick(uint8_t stat_id);

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

void ElementBlinkWallTick(uint8_t stat_id);

uint8_t ElementTransporterDraw(uint8_t x, uint8_t y);
void ElementTransporterTick(uint8_t stat_id);
void ElementTransporterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

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

void ElementSlimeTick(uint8_t stat_id);
void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

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

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCWTick(uint8_t stat_id);
uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCCWTick(uint8_t stat_id);

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y);
void ElementSpinningGunTick(uint8_t stat_id);

uint8_t ElementPusherDraw(uint8_t x, uint8_t y);
void ElementPusherTick(uint8_t stat_id);

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

void ElementCentipedeHeadTick(uint8_t stat_id);
void ElementCentipedeSegmentTick(uint8_t stat_id);

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
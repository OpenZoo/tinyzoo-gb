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

void ElementBoardEdgeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementMessageTimerTick(uint8_t stat_id) {
	// TODO
}

void ElementMonitorTick(uint8_t stat_id) {
	// TODO
}

uint8_t ElementPlayerDraw(uint8_t x, uint8_t y);
void ElementPlayerTick(uint8_t stat_id);

void ElementAmmoTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.ammo += 5;

	ZOO_TILE(x, y).element = E_EMPTY;
	game_update_sidebar_ammo();
	sound_queue(2, sound_ammo_pickup);

	// TODO
}

void ElementTorchTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.torches += 1;
	ZOO_TILE(x, y).element = E_EMPTY;
	
	board_draw_tile(x, y);
	game_update_sidebar_torches();

	// TODO

	sound_queue(3, sound_torch_pickup);
}

void ElementGemTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.gems += 1;
	zoo_world_info.health += 1;
	zoo_world_info.score += 10;

	ZOO_TILE(x, y).element = E_EMPTY;
	game_update_sidebar_health();
	game_update_sidebar_gems_time();
	game_update_sidebar_score();
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
			game_update_sidebar_keys();

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
			game_update_sidebar_keys();

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

void ElementPassageTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

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

// adds 81 bytes, performance untested
// #define UNROLL_LINE_DRAW

uint8_t ElementLineDraw(uint8_t x, uint8_t y) {
	uint8_t v = 0;
#ifdef UNROLL_LINE_DRAW
	uint8_t element = ZOO_TILE(x, y - 1).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 1;
	element = ZOO_TILE(x, y + 1).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 2;
	element = ZOO_TILE(x - 1, y).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 4;
	element = ZOO_TILE(x + 1, y).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 8;
#else
	uint8_t i, shift = 1;
	for (i = 0; i < 4; i++, shift <<= 1) {
		uint8_t element = ZOO_TILE(x + neighbor_delta_x[i], y + neighbor_delta_y[i]).element;
		if (element == E_LINE || element == E_BOARD_EDGE) {
			v |= shift;
		}
	}
#endif
	return line_tiles[v];
}

void ElementBearTick(uint8_t stat_id);

void ElementRuffianTick(uint8_t stat_id);

void ElementSlimeTick(uint8_t stat_id);
void ElementSlimeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void ElementSharkTick(uint8_t stat_id);

uint8_t ElementConveyorCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCWTick(uint8_t stat_id);
uint8_t ElementConveyorCCWDraw(uint8_t x, uint8_t y);
void ElementConveyorCCWTick(uint8_t stat_id);

uint8_t ElementSpinningGunDraw(uint8_t x, uint8_t y);
void ElementSpinningGunTick(uint8_t stat_id);

uint8_t ElementPusherDraw(uint8_t x, uint8_t y);
void ElementPusherTick(uint8_t stat_id);

void ElementLionTick(uint8_t stat_id);
void ElementTigerTick(uint8_t stat_id);

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
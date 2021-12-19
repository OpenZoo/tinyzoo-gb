#include "stdbool.h"
#pragma bank 1

#include "elements.h"
#include "elements_utils.h"
#include "gamevars.h"
#include "game.h"
#include "input.h"
#include "math.h"
#include "../res/message_consts.h"
#include "oop.h"
#include "sound_consts.h"
#include "timer.h"

#include "elements_funcdefs.h"

void ElementDefaultTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {

}

void ElementDamagingTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	board_attack(0, x, y);
}

void ElementMonitorTick(uint8_t stat_id) {
	// TODO
}

void ElementAmmoTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.ammo += 5;

	ZOO_TILE(x, y).element = E_EMPTY;
	game_update_sidebar_ammo();
	sound_queue(2, sound_ammo_pickup);

	if (!(msg_flags.f1 & MSG_FLAG1_AMMO)) {
		display_message(200, NULL, NULL, msg_ammo_pickup);
		msg_flags.f1 |= MSG_FLAG1_AMMO;
	}
}

void ElementTorchTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_world_info.torches += 1;
	ZOO_TILE(x, y).element = E_EMPTY;
	
	board_draw_tile(x, y);
	game_update_sidebar_torches();

	if (!(msg_flags.f1 & MSG_FLAG1_TORCH)) {
		display_message(200, msg_torch_pickup_line1, msg_torch_pickup_line2, msg_torch_pickup_line3);
		msg_flags.f1 |= MSG_FLAG1_TORCH;
	}

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

	if (!(msg_flags.f2 & MSG_FLAG2_GEM)) {
		display_message(200, NULL, msg_gem_pickup_line1, msg_gem_pickup_line2);
		msg_flags.f2 |= MSG_FLAG2_GEM;
	}
}

void ElementEnergizerTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	sound_queue(9, sound_energizer_loop);

	ZOO_TILE(x, y).element = E_EMPTY;
	board_draw_tile(x, y);

	zoo_world_info.energizer_ticks = 75;
	if (!(msg_flags.f2 & MSG_FLAG2_ENERGIZER)) {
		display_message(200, NULL, msg_energizer_pickup_line1, msg_energizer_pickup_line2);
		msg_flags.f2 |= MSG_FLAG2_ENERGIZER;
	}
		
	oop_send_target(OOP_TARGET_ALL, false, OOP_LABEL_ENERGIZE, false);
}

void ElementWaterTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	sound_queue(3, sound_water_splash);
	display_message(100, NULL, msg_water_touch_line1, msg_water_touch_line2);
}

void ElementForestTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ZOO_TILE(x, y).element = E_EMPTY;
	board_draw_tile(x, y);

	sound_queue(3, sound_forest);
	if (!(msg_flags.f1 & MSG_FLAG1_FOREST)) {
		display_message(200, NULL, msg_forest_touch_line1, msg_forest_touch_line2);
		msg_flags.f1 |= MSG_FLAG1_FOREST;
	}
}

void ElementPushableTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ElementPushablePush(x, y, *dx, *dy);
	sound_queue(2, sound_push);
}

void ElementFakeTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	if (!(msg_flags.f2 & MSG_FLAG2_FAKE)) {
		display_message(150, NULL, msg_fake_touch_line1, msg_fake_touch_line2);
		msg_flags.f2 |= MSG_FLAG2_FAKE;
	}
}

void ElementInvisibleTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	ZOO_TILE(x, y).element = E_NORMAL;
	board_draw_tile(x, y);

	sound_queue(3, sound_invisible);
	display_message(100, NULL, msg_invisible_touch_line1, msg_invisible_touch_line2);
}

#include "element_defs_drawprocs.inc"
#include "element_defs_tickprocs.inc"
#include "element_defs_touchprocs.inc"
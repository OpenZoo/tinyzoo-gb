#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

static const char *msg_key_pickup_tbl[] = {
	msg_key_pickup_08,
	msg_key_pickup_09,
	msg_key_pickup_0a,
	msg_key_pickup_0b,
	msg_key_pickup_0c,
	msg_key_pickup_0d,
	msg_key_pickup_0e,
	msg_key_pickup_0f
};

static const char *msg_door_open_tbl[] = {
	msg_door_open_08,
	msg_door_open_09,
	msg_door_open_0a,
	msg_door_open_0b,
	msg_door_open_0c,
	msg_door_open_0d,
	msg_door_open_0e,
	msg_door_open_0f
};

void ElementKeyTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color & 7);
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) != 0) {
			display_message(200, NULL, msg_key_pickup_no, msg_key_pickup_08);
			sound_queue(2, sound_key_failure);
		} else {
			zoo_world_info.gems |= 0x100;
			tile->element = E_EMPTY;
			game_update_sidebar_gems_time();

			display_message(200, NULL, msg_key_pickup_yes, msg_key_pickup_08);
			sound_queue(2, sound_key_success);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (zoo_world_info.keys & key_shift) {
			display_message(200, NULL, msg_key_pickup_no, msg_key_pickup_tbl[key]);
			sound_queue(2, sound_key_failure);
		} else {
			zoo_world_info.keys |= key_shift;
			tile->element = E_EMPTY;
			game_update_sidebar_keys();

			display_message(200, NULL, msg_key_pickup_yes, msg_key_pickup_tbl[key]);
			sound_queue(2, sound_key_success);
		}
	}
}

void ElementDoorTouch(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy) {
	zoo_tile_t *tile = &ZOO_TILE(x, y);
	uint8_t key = (tile->color >> 4) & 7;
	if (key == 0) {
#ifdef FEAT_BLACK_KEYS
		if ((zoo_world_info.gems & 0xFF00) == 0) {
			display_message(200, NULL, msg_door_open_08, msg_door_open_no);
			sound_queue(3, sound_door_failure);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.gems &= 0x00FF;
			game_update_sidebar_gems_time();

			display_message(200, NULL, msg_door_open_08, msg_door_open_yes);
			sound_queue(3, sound_door_success);
		}
#endif
	} else {
		uint8_t key_shift = 1 << key;

		if (!(zoo_world_info.keys & key_shift)) {
			display_message(200, NULL, msg_door_open_tbl[key], msg_door_open_no);
			sound_queue(3, sound_door_failure);
		} else {
			tile->element = E_EMPTY;
			board_draw_tile(x, y);

			zoo_world_info.keys &= ~key_shift;
			game_update_sidebar_keys();

			display_message(200, NULL, msg_door_open_tbl[key], msg_door_open_yes);
			sound_queue(3, sound_door_success);
		}
	}
}

// gamevars.h - game state variables

#ifndef __GAMEVARS_H__
#define __GAMEVARS_H__

#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "himem.h"
#include "renderer.h"

typedef struct {
	void *ptr;
	uint8_t bank;
} far_ptr_t;

#define SHOT_SOURCE_PLAYER 0
#define SHOT_SOURCE_ENEMY 1

#define VIEWPORT_CENTER_X ((VIEWPORT_WIDTH - 1) >> 1)
#define VIEWPORT_CENTER_Y ((VIEWPORT_HEIGHT - 1) >> 1)
#define VIEWPORT_PLAYER_OFFSET 1
#define VIEWPORT_PLAYER_MIN_X (VIEWPORT_CENTER_X - VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MAX_X (VIEWPORT_CENTER_X + VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MIN_Y (VIEWPORT_CENTER_Y - VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MAX_Y (VIEWPORT_CENTER_Y + VIEWPORT_PLAYER_OFFSET)

#define STAT_ID_NONE 255
#define STAT_ID_NONE_MIN 254

#define COLOR_SPECIAL_MIN 0xF0
#define COLOR_CHOICE_ON_BLACK 0xFF
#define COLOR_WHITE_ON_CHOICE 0xFE
#define COLOR_CHOICE_ON_CHOICE 0xFD

typedef struct {
	uint8_t element;
	uint8_t color;
} zoo_tile_t;

// 16 bytes per stat
typedef struct {
	uint8_t x, y;
	int8_t step_x, step_y;
	uint8_t cycle;
	uint8_t p1, p2, p3;
	uint8_t follower, leader;
	zoo_tile_t under;
	uint16_t data_ofs;
	uint16_t data_pos;
} zoo_stat_t;

#define ELEMENT_DESTRUCTIBLE 0x01
#define ELEMENT_PUSHABLE 0x02
#define ELEMENT_VISIBLE_IN_DARK 0x04
#define ELEMENT_PLACEABLE_ON_TOP 0x08
#define ELEMENT_WALKABLE 0x10
#define ELEMENT_TYPICALLY_STATTED 0x20
#define ELEMENT_TYPICALLY_TEXTED 0x40

typedef uint8_t (*zoo_element_draw_proc)(uint8_t x, uint8_t y);
typedef void (*zoo_element_tick_proc)(uint8_t stat_id);
typedef void (*zoo_element_touch_proc)(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

#define BOARD_IS_DARK 0x01
#define BOARD_REENTER_WHEN_ZAPPED 0x02

typedef struct {
	uint8_t max_shots;
	uint8_t flags;
	uint8_t neighbor_boards[4];
	uint8_t start_player_x;
	uint8_t start_player_y;
	int16_t time_limit_sec;
} zoo_board_info_t;

typedef struct {
	int16_t ammo;
	int16_t gems;
	uint8_t keys;
	int16_t health;
	int16_t torches;
	int16_t score;
	uint8_t current_board;
	uint8_t torch_ticks;
	uint8_t energizer_ticks;
	uint8_t oop_flags[MAX_FLAG];
	int16_t board_time_sec;
	int16_t board_time_hsec;
	uint8_t board_count;
} zoo_world_info_t;

#define MSG_FLAG1_AMMO 0x01
#define MSG_FLAG1_OUT_OF_AMMO 0x02
#define MSG_FLAG1_NO_SHOOTING 0x04
#define MSG_FLAG1_TORCH 0x08
#define MSG_FLAG1_OUT_OF_TORCHES 0x10
#define MSG_FLAG1_ROOM_NOT_DARK 0x20
#define MSG_FLAG1_HINT_TORCH 0x40
#define MSG_FLAG1_FOREST 0x80
#define MSG_FLAG2_FAKE 0x01
#define MSG_FLAG2_GEM 0x02
#define MSG_FLAG2_ENERGIZER 0x04

typedef struct {
	uint8_t f1, f2;
} zoo_message_flags_t;

typedef struct {
	uint8_t world_id;
	bool paused;
	bool play_exit_requested;
	uint16_t tick_time_counter;
	uint16_t current_tick;
	uint8_t current_stat_ticked;
	uint8_t game_state_element;
	uint8_t tick_time_duration;
} zoo_game_state_t;

#define ZOO_ELEMENT_BOUNDS(id) ((id) <= MAX_ELEMENT)
#define ZOO_TILE_READBOUNDS(x, y) ( ((x) >= 0) && ((y) >= 0) && ((x) <= 61) && ((y) <= 26) )
#ifdef OPT_BOARD_EDGE_IMMUTABLE
extern zoo_tile_t zoo_tiles[62 * 26];
#define ZOO_TILE_WRITEBOUNDS(x, y) ( ((x) >= 0) && ((y) >= 0) && ((x) <= 61) && ((y) < 26) )
#else
extern zoo_tile_t zoo_tiles[62 * 27];
#define ZOO_TILE_WRITEBOUNDS(x, y) ( ((x) >= 0) && ((y) >= 0) && ((x) <= 61) && ((y) <= 26) )
#endif

#ifndef __GAMEVARS_INTERNAL__
extern zoo_board_info_t zoo_board_info;
extern zoo_world_info_t zoo_world_info;
extern zoo_tile_t *const zoo_tiles_y[27];
extern zoo_message_flags_t msg_flags;
#ifndef SM83
extern uint8_t zoo_stat_count;
#endif
extern uint16_t zoo_stat_data_size;
extern uint8_t zoo_stat_data[MAX_DATA_OFS_SIZE];
extern zoo_stat_t zoo_stats[MAX_STAT + 3];
extern zoo_game_state_t zoo_game_state;
#endif

#define ZOO_TILE(x, y) zoo_tiles_y[(uint8_t)(y)][(uint8_t)(x)]
#define ZOO_TILE_COPY(v, w) *((uint16_t*) &(v)) = *((uint16_t*) &(w))
#define ZOO_TILE_ASSIGN(v, x, y) *((uint16_t*) &(v)) = *((uint16_t*) &ZOO_TILE((uint8_t)(x), (uint8_t)(y)))
#define ZOO_TILE_CHANGE2(x, y, el, col) ((uint16_t*) zoo_tiles_y[(uint8_t)(y)])[(uint8_t)(x)] = ((uint16_t) ((el) | ((col) << 8)))
#define ZOO_STAT(id) zoo_stats[(uint8_t) ((id) + 1)]
#define ZOO_STAT_AT(x, y) zoo_stats[get_stat_id_at((x), (y)) + 1]

#define ZOO_BUSYLOOP(cond) while ((cond)) { \
	__asm \
		halt \
	__endasm; \
}

#endif

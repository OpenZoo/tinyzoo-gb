// gamevars.h - game state variables

#ifndef __GAMEVARS_H__
#define __GAMEVARS_H__

#include <stdbool.h>
#include <stdint.h>
#include "renderer.h"

#define SHOT_SOURCE_PLAYER 0
#define SHOT_SOURCE_ENEMY 1

#define VIEWPORT_CENTER_X ((VIEWPORT_WIDTH - 1) >> 1)
#define VIEWPORT_CENTER_Y ((VIEWPORT_HEIGHT - 1) >> 1)
#define VIEWPORT_PLAYER_OFFSET 2
#define VIEWPORT_PLAYER_MIN_X (VIEWPORT_CENTER_X - VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MAX_X (VIEWPORT_CENTER_X + VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MIN_Y (VIEWPORT_CENTER_Y - VIEWPORT_PLAYER_OFFSET)
#define VIEWPORT_PLAYER_MAX_Y (VIEWPORT_CENTER_Y + VIEWPORT_PLAYER_OFFSET)

#define TORCH_DX 8
#define TORCH_DY 5
#define TORCH_DIST_SQR 50

#define STAT_ID_NONE 255

#define COLOR_SPECIAL_MIN 0xF0
#define COLOR_CHOICE_ON_BLACK 0xFF
#define COLOR_WHITE_ON_CHOICE 0xFE
#define COLOR_CHOICE_ON_CHOICE 0xFD

#define MAX_STAT 150
#define MAX_ELEMENT 53
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 25
#define MAX_DATA_OFS_SIZE (MAX_STAT * 3)

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
	// TODO: Data
	uint16_t data_ofs; // first byte = program ID; remaining bytes = label flags
	uint16_t data_pos;
} zoo_stat_t;

#define ELEMENT_DESTRUCTIBLE 0x01
#define ELEMENT_PUSHABLE 0x02
#define ELEMENT_VISIBLE_IN_DARK 0x04
#define ELEMENT_PLACEABLE_ON_TOP 0x08
#define ELEMENT_WALKABLE 0x10

typedef uint8_t (*zoo_element_draw_proc)(uint8_t x, uint8_t y);
typedef void (*zoo_element_tick_proc)(uint8_t stat_id);
typedef void (*zoo_element_touch_proc)(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

typedef struct {
	uint8_t character;
	uint8_t color;
	uint8_t flags;
	zoo_element_draw_proc draw_proc;
	zoo_element_tick_proc tick_proc;
	zoo_element_touch_proc touch_proc;
	uint8_t score_value;
} zoo_element_def_t;

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
	// TODO: Flags
	int16_t board_time_sec;
	int16_t board_time_hsec;
} zoo_world_info_t;

typedef struct {
	bool paused;
	bool play_exit_requested;
	uint16_t tick_time_counter;
	uint16_t current_tick;
	uint8_t current_stat_ticked;
} zoo_game_state_t;

#ifndef __GAMEVARS_INTERNAL__
extern zoo_board_info_t zoo_board_info;
extern zoo_world_info_t zoo_world_info;
extern zoo_tile_t zoo_tiles[64 * 27];
extern uint8_t zoo_stat_count;
extern uint8_t zoo_stat_data[MAX_DATA_OFS_SIZE];
extern zoo_stat_t zoo_stats[MAX_STAT + 3];
extern zoo_game_state_t zoo_game_state;
#endif

#define ZOO_TILE(x, y) zoo_tiles[((y) << 6) | (x)]
#define ZOO_TILE_COPY(v, w) *((uint16_t*) &(v)) = *((uint16_t*) &(w))
#define ZOO_TILE_ASSIGN(v, x, y) *((uint16_t*) &(v)) = *((uint16_t*) &ZOO_TILE((x), (y)))
#define ZOO_STAT(id) zoo_stats[((id) + 1)]
#define ZOO_STAT_AT(x, y) zoo_stats[get_stat_id_at((x), (y)) + 1]

#endif

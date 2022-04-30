#include "elements.h"
#define __GAMEVARS_INTERNAL__
#include "gamevars.h"

zoo_board_info_t zoo_board_info;
zoo_world_info_t zoo_world_info;
zoo_message_flags_t msg_flags;
// uint8_t zoo_stat_count; - himem.h
zoo_stat_t zoo_stats[MAX_STAT + 3];
zoo_game_state_t zoo_game_state;
uint8_t zoo_stat_data[MAX_DATA_OFS_SIZE];
uint16_t zoo_stat_data_size;

#ifdef OPT_BOARD_EDGE_IMMUTABLE
static const uint8_t zoo_tile_edge_immutable[] = {
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0, E_BOARD_EDGE, 0,
	E_BOARD_EDGE, 0, E_BOARD_EDGE, 0
};

zoo_tile_t zoo_tiles[62 * 26];
zoo_tile_t *const zoo_tiles_y[27] = {
	zoo_tiles,
	zoo_tiles + (62 * 1),
	zoo_tiles + (62 * 2),
	zoo_tiles + (62 * 3),
	zoo_tiles + (62 * 4),
	zoo_tiles + (62 * 5),
	zoo_tiles + (62 * 6),
	zoo_tiles + (62 * 7),
	zoo_tiles + (62 * 8),
	zoo_tiles + (62 * 9),
	zoo_tiles + (62 * 10),
	zoo_tiles + (62 * 11),
	zoo_tiles + (62 * 12),
	zoo_tiles + (62 * 13),
	zoo_tiles + (62 * 14),
	zoo_tiles + (62 * 15),
	zoo_tiles + (62 * 16),
	zoo_tiles + (62 * 17),
	zoo_tiles + (62 * 18),
	zoo_tiles + (62 * 19),
	zoo_tiles + (62 * 20),
	zoo_tiles + (62 * 21),
	zoo_tiles + (62 * 22),
	zoo_tiles + (62 * 23),
	zoo_tiles + (62 * 24),
	zoo_tiles + (62 * 25),
	(zoo_tile_t*) zoo_tile_edge_immutable
};
#else
zoo_tile_t zoo_tiles[62 * 27];
zoo_tile_t *const zoo_tiles_y[27] = {
	zoo_tiles,
	zoo_tiles + (62 * 1),
	zoo_tiles + (62 * 2),
	zoo_tiles + (62 * 3),
	zoo_tiles + (62 * 4),
	zoo_tiles + (62 * 5),
	zoo_tiles + (62 * 6),
	zoo_tiles + (62 * 7),
	zoo_tiles + (62 * 8),
	zoo_tiles + (62 * 9),
	zoo_tiles + (62 * 10),
	zoo_tiles + (62 * 11),
	zoo_tiles + (62 * 12),
	zoo_tiles + (62 * 13),
	zoo_tiles + (62 * 14),
	zoo_tiles + (62 * 15),
	zoo_tiles + (62 * 16),
	zoo_tiles + (62 * 17),
	zoo_tiles + (62 * 18),
	zoo_tiles + (62 * 19),
	zoo_tiles + (62 * 20),
	zoo_tiles + (62 * 21),
	zoo_tiles + (62 * 22),
	zoo_tiles + (62 * 23),
	zoo_tiles + (62 * 24),
	zoo_tiles + (62 * 25),
	zoo_tiles + (62 * 26)
};
#endif

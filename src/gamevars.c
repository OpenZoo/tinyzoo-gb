#define __GAMEVARS_INTERNAL__
#include "gamevars.h"

zoo_board_info_t zoo_board_info;
zoo_world_info_t zoo_world_info;
zoo_tile_t zoo_tiles[64 * 27];
uint8_t zoo_stat_count;
zoo_stat_t zoo_stats[MAX_STAT + 3];
zoo_game_state_t zoo_game_state;
uint8_t zoo_stat_data[MAX_DATA_OFS_SIZE];
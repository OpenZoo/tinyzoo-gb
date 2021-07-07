#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

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
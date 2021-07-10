#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gb/gb.h>
#include "gamevars.h"
#include "game.h"
#include "renderer_sidebar.h"

void game_update_sidebar(void) {
	if (zoo_world_info.health < 0) {
		zoo_world_info.health = 0;
	}

	sidebar_draw_panel(0 >> 1, 2, 3, zoo_world_info.health, false);
	sidebar_draw_panel(6 >> 1, 132, 3, zoo_world_info.ammo, false);
	if (zoo_board_info.time_limit_sec > 0) {
		sidebar_draw_panel(12 >> 1, 'T', 3, zoo_board_info.time_limit_sec - zoo_world_info.board_time_sec, false);
	} else {
		sidebar_draw_panel(12 >> 1, 4, 2, zoo_world_info.gems, false);
	}
	sidebar_draw_panel(18 >> 1, 157, 1, zoo_world_info.torches, false);

	sidebar_draw_panel(24 >> 1, '+', 3, zoo_world_info.score, true);

	sidebar_draw_keys(32 >> 1, zoo_world_info.keys);
}
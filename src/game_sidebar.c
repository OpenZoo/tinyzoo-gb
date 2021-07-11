#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gb/gb.h>
#include "gamevars.h"
#include "game.h"
#include "renderer_sidebar.h"

void game_update_sidebar_health(void) {
	if (zoo_world_info.health < 0) {
		zoo_world_info.health = 0;
	}

	sidebar_draw_panel(0 >> 1, 2, 3, zoo_world_info.health, false);
}

void game_update_sidebar_ammo(void) {
	sidebar_draw_panel(6 >> 1, 132, 3, zoo_world_info.ammo, false);
}

void game_update_sidebar_gems_time(void) {
	if (zoo_board_info.time_limit_sec > 0) {
		sidebar_draw_panel(12 >> 1, 'T', 3, zoo_board_info.time_limit_sec - zoo_world_info.board_time_sec, false);
	} else {
		sidebar_draw_panel(12 >> 1, 4, 2, zoo_world_info.gems, false);
	}
}

void game_update_sidebar_torches(void) {
	sidebar_draw_panel(18 >> 1, 157, 1, zoo_world_info.torches, false);
}

void game_update_sidebar_score(void) {
	sidebar_draw_panel(24 >> 1, '+', 3, zoo_world_info.score, true);
}

void game_update_sidebar_keys(void) {
	sidebar_draw_keys(32 >> 1, zoo_world_info.keys);
}

void game_update_sidebar_all(void) {
	game_update_sidebar_health();
	game_update_sidebar_ammo();
	game_update_sidebar_gems_time();
	game_update_sidebar_torches();
	game_update_sidebar_score();
	game_update_sidebar_keys();
}
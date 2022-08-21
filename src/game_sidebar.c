#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gbdk/platform.h>
#include "gamevars.h"
#include "game.h"
#include "renderer_sidebar.h"

void game_update_sidebar_health(void) {
#ifndef HACK_HIDE_STATUSBAR
	if (zoo_world_info.health < 0) {
		zoo_world_info.health = 0;
	}

	sidebar_draw_panel(0 >> 1, 2, 3, zoo_world_info.health, false);
#endif
}

void game_update_sidebar_ammo(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(6 >> 1, 132, 3, zoo_world_info.ammo, false);
#endif
}

void game_update_sidebar_gems_time(void) {
#ifndef HACK_HIDE_STATUSBAR
	if (zoo_board_info.time_limit_sec > 0) {
		sidebar_draw_panel(12 >> 1, 'T', 3, zoo_board_info.time_limit_sec - zoo_world_info.board_time_sec, false);
	} else {
		sidebar_draw_panel(12 >> 1, 4, 2, zoo_world_info.gems, false);
	}
#endif
}

void game_update_sidebar_torches(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(18 >> 1, 157, 1, zoo_world_info.torches, false);
#endif
}

void game_update_sidebar_score(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_panel(24 >> 1, '+', 3, zoo_world_info.score, true);
#endif
}

void game_update_sidebar_keys(void) {
#ifndef HACK_HIDE_STATUSBAR
	sidebar_draw_keys(32 >> 1, zoo_world_info.keys);
#endif
}

void game_update_sidebar_all(void) {
#ifndef HACK_HIDE_STATUSBAR
	game_update_sidebar_health();
	game_update_sidebar_ammo();
	game_update_sidebar_gems_time();
	game_update_sidebar_torches();
	game_update_sidebar_score();
	game_update_sidebar_keys();
#endif
}

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

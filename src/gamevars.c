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

#include "elements.h"
#define __GAMEVARS_INTERNAL__
#include "gamevars.h"

zoo_board_info_t zoo_board_info;
zoo_world_info_t zoo_world_info;
zoo_message_flags_t msg_flags;
#ifndef SM83
uint8_t zoo_stat_count;
#endif
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

#ifdef SHOW_CHEATS
uint8_t cheat_active = 0;
#endif

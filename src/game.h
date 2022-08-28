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

#ifndef __GAME_H__
#define __GAME_H__

#include <stdint.h>
#include <gbdk/platform.h>
#include "gamevars.h"

#define VIEWPORT_MIN_X 1
#define VIEWPORT_MAX_X (BOARD_WIDTH - VIEWPORT_WIDTH + 1)
#define VIEWPORT_MIN_Y 1
#define VIEWPORT_MAX_Y (BOARD_HEIGHT - VIEWPORT_HEIGHT + 1)

extern const zoo_stat_t stat_template_default;
#if defined(NINTENDO)
AT(0x0068) extern const int8_t neighbor_delta_x[4];
AT(0x006C) extern const int8_t neighbor_delta_y[4];
AT(0x0070) extern const int8_t diagonal_delta_x[8];
AT(0x0078) extern const int8_t diagonal_delta_y[8];
#else
extern const int8_t neighbor_delta_x[4];
extern const int8_t neighbor_delta_y[4];
extern const int8_t diagonal_delta_x[8];
extern const int8_t diagonal_delta_y[8];
#endif

extern int8_t viewport_x;
extern int8_t viewport_y;
extern uint8_t viewport_focus_stat;
extern bool viewport_focus_locked;

// game.c (bank 0)

void center_viewport_on(int8_t x, int8_t y);
void viewport_reset(void);
bool viewport_request_player_focus(void);

void board_redraw(void);

void board_enter_stage1(void); // before transition
void board_enter_stage2(void); // during transition
void board_enter_stage3(void); // after transition
void board_change(uint8_t id);
void board_undraw_tile(uint8_t x, uint8_t y);
void board_draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
void board_draw_tile(uint8_t x, uint8_t y);
uint8_t get_stat_id_at(uint8_t x, uint8_t y);
void add_stat(uint8_t tx, uint8_t ty, uint8_t element, uint8_t color, uint8_t cycle, const zoo_stat_t *template);
void remove_stat(uint8_t stat_id);
void move_stat(uint8_t stat_id, uint8_t x, uint8_t y);
void damage_stat(uint8_t stat_id);
void board_damage_tile(uint8_t x, uint8_t y);
void calc_direction_rnd(int8_t *dx, int8_t *dy);
void calc_direction_seek(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void init_display_message(uint8_t time, bool visible);
void display_message(uint8_t time, const char* line1, const char* line2, const char* line3); // bank 3 only

// game_sidebar.c (bank 0)

void game_update_sidebar_all(void);
void game_update_sidebar_health(void);
void game_update_sidebar_ammo(void);
void game_update_sidebar_gems_time(void);
void game_update_sidebar_torches(void);
void game_update_sidebar_score(void);
void game_update_sidebar_keys(void);

// game_bank1.c (bank 1)

void board_attack(uint8_t stat_id, uint8_t x, uint8_t y);
void game_play_loop(bool board_changed) BANKED;

// game_bank2.c (bank 2)

void board_create(void) BANKED;
void world_create(void) BANKED;
void scroll_viewport_to(uint8_t vx, uint8_t vy, bool force_redraw);
void game_scrolling_view(void) BANKED;
void move_stat_scroll_focused(uint8_t stat_id, uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y, bool force) BANKED;
bool board_shoot(uint8_t element, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t source) BANKED;

// game_menus.c (bank 2)

void game_menu_act_enter_world(uint8_t world_id, bool new_game, bool first_launch) BANKED;
bool game_pause_menu(void) BANKED;

#endif

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

#ifndef __SOUND_CONSTS_H__
#define __SOUND_CONSTS_H__

#include <stdint.h>

extern const uint8_t sound_transporter_move[];
extern const uint8_t sound_ricochet[];
extern const uint8_t sound_damage[];
extern const uint8_t sound_player_zapped[];
extern const uint8_t sound_player_damage[];
extern const uint8_t sound_player_shoot[];
extern const uint8_t sound_player_game_over[];
extern const uint8_t sound_time_running_out[];
extern const uint8_t sound_destroy_bullet[];
extern const uint8_t sound_destroy[];
extern const uint8_t sound_push[];
extern const uint8_t sound_forest[];
extern const uint8_t sound_invisible[];
extern const uint8_t sound_key_failure[];
extern const uint8_t sound_key_success[];
extern const uint8_t sound_door_failure[];
extern const uint8_t sound_door_success[];
extern const uint8_t sound_ammo_pickup[];
extern const uint8_t sound_gem_pickup[];
extern const uint8_t sound_torch_burnout[];
extern const uint8_t sound_torch_pickup[];
extern const uint8_t sound_water_splash[];
extern const uint8_t sound_bomb_activated[];
extern const uint8_t sound_bomb_explosion[];
extern const uint8_t sound_bomb_tick1[];
extern const uint8_t sound_bomb_tick2[];
extern const uint8_t sound_slime_touch[];
extern const uint8_t sound_energizer_loop[];
extern const uint8_t sound_energizer_finish[];
extern const uint8_t sound_duplicator_success[];
extern const uint8_t sound_duplicator_failure[];
extern const uint8_t sound_passage_teleport[];
extern const uint8_t sound_scroll_touch[];

#endif /* __SOUND_CONSTS_H__ */

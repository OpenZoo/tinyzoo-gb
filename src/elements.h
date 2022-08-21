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

#ifndef __ELEMENTS_H__
#define __ELEMENTS_H__

#include <stdint.h>
#include "gamevars.h"

extern const uint8_t zoo_element_defs_character[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_color[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_flags[MAX_ELEMENT + 1]; // bank 0
extern const zoo_element_draw_proc zoo_element_defs_drawprocs[MAX_ELEMENT + 1]; // bank 1
extern const zoo_element_tick_proc zoo_element_defs_tickprocs[MAX_ELEMENT + 1]; // bank 1
extern const zoo_element_touch_proc zoo_element_defs_touchprocs[MAX_ELEMENT + 1]; // bank 1
extern const uint8_t zoo_element_defs_scorevalues[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_cycles[MAX_ELEMENT + 1]; // bank 2

#define E_EMPTY 0
#define E_BOARD_EDGE 1
#define E_MESSAGE_TIMER 2
#define E_MONITOR 3
#define E_PLAYER 4
#define E_AMMO 5
#define E_TORCH 6
#define E_GEM 7
#define E_KEY 8
#define E_DOOR 9
#define E_SCROLL 10
#define E_PASSAGE 11
#define E_DUPLICATOR 12
#define E_BOMB 13
#define E_ENERGIZER 14
#define E_STAR 15
#define E_CONVEYOR_CW 16
#define E_CONVEYOR_CCW 17
#define E_BULLET 18
#define E_WATER 19
#define E_FOREST 20
#define E_SOLID 21
#define E_NORMAL 22
#define E_BREAKABLE 23
#define E_BOULDER 24
#define E_SLIDER_NS 25
#define E_SLIDER_EW 26
#define E_FAKE 27
#define E_INVISIBLE 28
#define E_BLINK_WALL 29
#define E_TRANSPORTER 30
#define E_LINE 31
#define E_RICOCHET 32
#define E_BLINK_RAY_EW 33
#define E_BEAR 34
#define E_RUFFIAN 35
#define E_OBJECT 36
#define E_SLIME 37
#define E_SHARK 38
#define E_SPINNING_GUN 39
#define E_PUSHER 40
#define E_LION 41
#define E_TIGER 42
#define E_BLINK_RAY_NS 43
#define E_CENTIPEDE_HEAD 44
#define E_CENTIPEDE_SEGMENT 45
#define E_TEXT_BLUE 47
#define E_TEXT_GREEN 48
#define E_TEXT_CYAN 49
#define E_TEXT_RED 50
#define E_TEXT_PURPLE 51
#define E_TEXT_YELLOW 52
#define E_TEXT_WHITE 53

#define E_TEXT_MIN E_TEXT_BLUE

#endif

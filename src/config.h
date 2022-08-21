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

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "platform_config.h"

#define MAX_BOARD 101

// (Moved to platform_config.h)
// Use accurate RNG. Very slow, but matches ZZT's original behaviour.
// #define USE_ACCURATE_RNG
// Use XorShift-based RNG by John Metcalf. Fast. SM83-only.
// #define USE_XORSHIFT_RNG
// Use RNG by Damian Yerrick. Faster. SM83-only.
// #define USE_YERRICK_RNG

// #define FEAT_BLACK_KEYS // Black keys/doors compat - costs ~235 bytes (as of 18/12/2021)
#define FEAT_BOARD_TRANSITIONS
#define BUGFIX_BLINKWALL_OFFSET
// #define BUGFIX_DIE_UNDER
#define BUGFIX_DIEMOVE_SCROLL
#define BUGFIX_DIEMOVE_OBJECT
#define BUGFIX_DIEMOVE_OOP_EXEC
#define BUGFIX_DIEMOVE_MESSAGE
// #define BUGFIX_KOOPO_BUG
// #define BUGFIX_PUT_RANGE

// adds 81 bytes, performance untested
// #define OPT_UNROLL_LINE_DRAW

// saves 124 (!!!) bytes of RAM; however, loses 124 bytes of bank0 ROM
// this makes some parts of the board edge immutable
#define OPT_BOARD_EDGE_IMMUTABLE

// #define RESET_SAVE_ON_START
#define SHOW_TITLE
// #define HACK_HIDE_STATUSBAR
// #define HACK_PLAYER_INVINCIBLE

#define MAX_SCROLL_DISTANCE_BEFORE_REDRAW 15

#define TORCH_DX 8
// #define TORCH_DY 5
#define TORCH_DY 4
#define TORCH_DIST_SQR 50
#define TORCH_DURATION 200
#define MAX_FLAG 10
#define MAX_STAT 150
#define MAX_ELEMENT 53
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 25
#define MAX_OOP_INSTRUCTION_COUNT 32

#define MAX_DATA_OFS_SIZE 600
#define MAX_SOUND_BUFFER_SIZE 254
#define MAX_TEXT_WINDOW_LINES 512

// #define DEBUG_PRINTFS
// #define DEBUG_PRINTFS_OOP_EXEC
// #define DEBUG_SRAM_WRITES
// #define DEBUG_SOUND_QUEUE

#endif /* __CONFIG_H__ */

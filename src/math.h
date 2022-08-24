/**
 * TinyZoo/GB utility code
 *
 * Copyright (c) 2021, 2022 Adrian Siekierka
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef __MATH_H__
#define __MATH_H__

#include "config.h"

#include <stdint.h>
#include <stdlib.h>

#if defined(SM83)
int8_t signum8(int8_t x) PRESERVES_REGS(b, c, d, h, l);
int8_t difference8(int8_t a, int8_t b) PRESERVES_REGS(b, c, h, l);
#else
int8_t signum8(int8_t x);
int8_t difference8(int8_t a, int8_t b);
#endif

#if defined(SM83)
// Optimized modulo routines. Designed for small dividends (<= 9-bit).
uint8_t zoo_modu16_8(uint16_t a, uint8_t b) PRESERVES_REGS(c);
uint8_t zoo_mods16_8(int16_t a, uint8_t b);
#else
#define zoo_modu16_8(a, b) ((uint8_t) (((uint16_t) (a)) % ((uint8_t) (b))))
#define zoo_mods16_8(a, b) ((uint8_t) (((int16_t) (a)) % ((uint8_t) (b))))
#endif

#if defined(SM83)
int16_t rand(void) PRESERVES_REGS(d, e);
uint8_t rand_mask8(uint8_t max) PRESERVES_REGS(d, e);
int16_t rand_mod(int16_t max);
uint8_t rand_np1(uint8_t max) PRESERVES_REGS(c);
#else
int16_t rand(void);
uint8_t rand_mask8(uint8_t max);
int16_t rand_mod(int16_t max);
#define rand_np1 rand_mod
#endif
void srand(uint16_t seed);

#define RAND2() rand_mask8(1)
#define RAND3() rand_np1(3)
#define RAND4() rand_mask8(3)
#define RAND7() rand_mod(7)
#define RAND9() rand_np1(9)
#define RAND10() rand_mod(10)
#define RAND17() rand_np1(17)

#endif /* __MATH_H__ */

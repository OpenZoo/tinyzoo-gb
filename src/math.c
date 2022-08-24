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

#include <stdlib.h>
#include "math.h"

// helper functions

#if !defined(SM83)
int8_t signum8(int8_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

int8_t difference8(int8_t a, int8_t b) {
	return abs(a - b);
}
#endif

// RNG

uint32_t rand_seed;

void srand(uint16_t seed) {
	rand_seed = seed == 0 ? 1 : seed;
}

#if !defined(SM83)
static void rand_step(void) {
	rand_seed = (rand_seed * 134775813) + 1;
}

int16_t rand(void) {
	rand_step();
	return (rand_seed >> 16);
}

int16_t rand_mask(int16_t max) {
	rand_step();
	return (rand_seed >> 16) & max;
}

int16_t rand_mod(int16_t max) {
	rand_step();
	return (rand_seed >> 16) % max;
}
#endif

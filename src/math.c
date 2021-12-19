#include <stdlib.h>
#include "math.h"

// helper functions

#ifdef GBZ80
int8_t signum8(int8_t x) __naked __preserves_regs(b, c, d) {
	x;
__asm
	ldhl sp, #2
	ld e, (hl)
	bit 7, e
	jr nz, Signum8ReturnMinusOne ; x < 0
	xor a, a
	cp a, e ; x == 0?
	ret z
	ld e, #0x01 ; x > 0
	ret
Signum8ReturnMinusOne:
	ld e, #0xFF
	ret
__endasm;
}

int8_t difference8(int8_t a, int8_t b) __naked __preserves_regs(b, c, d) {
	a, b;
__asm
	ldhl sp, #2
	ld a, (hl+)
	ld e, (hl)
	cp a, e
	jr nc, Diff8NoSwap

	ld e, a
	ld a, (hl)

Diff8NoSwap:
	sub a, e
	ld e, a
	ret
__endasm;
}
#else
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

/* int8_t signum16(int16_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

int16_t difference16(int16_t a, int16_t b) {
	return abs(a - b);
} */

// RNG

#if defined(USE_XORSHIFT_RNG)
static uint16_t rand_seed;
#else
static uint32_t rand_seed;
#endif

void srand(uint16_t seed) {
	rand_seed = seed == 0 ? 1 : seed;
}

#if defined(USE_ACCURATE_RNG)
int16_t rand(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) % max;
}

int16_t rand_mask(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) & max;
}
#elif defined(USE_XORSHIFT_RNG)
// Implementation based on John Metcalf's work
// Permission: https://twitter.com/john_metcalf/status/1446544969506906113

int16_t rand(int16_t max) {
	max;
__asm
	ld a, (_rand_seed)
	ld l, a
	ld a, (_rand_seed+1)
	ld h, a
	rra
	ld a, l
	rra
	xor a, h
	ld h, a
	ld a, l
	rra
	ld a, h
	rra
	xor a, l
	ld e, a
	ld (_rand_seed), a
	xor a, h
	ld d, a
	ld (_rand_seed+1), a
	ldhl	sp,	#2
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	push	bc
	push	de
	call	__moduint
	add	sp, #4
__endasm;
}

int16_t rand_mask(int16_t max) __preserves_regs(b, c) {
	max;
__asm
	ld a, (_rand_seed)
	ld l, a
	ld a, (_rand_seed+1)
	ld h, a
	rra
	ld a, l
	rra
	xor a, h
	ld h, a
	ld a, l
	rra
	ld a, h
	rra
	xor a, l
	ld (_rand_seed), a
	ld e, a
	xor a, h
	ld (_rand_seed+1), a
	ld d, a
	ldhl sp, #2
	ld a, (hl+)
	and a, e
	ld e, a
	ld a, (hl)
	and a, d
	ld d, a
__endasm;
}
#elif defined(USE_YERRICK_RNG)
// https://github.com/pinobatch/libbet/blob/5b099093a22e7241a6405244d301ca5ba08db140/src/rand.z80
/*
 Pseudorandom number generator

 Copyright 2018, 2020 Damian Yerrick

 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.
 2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.
 3. This notice may not be removed or altered from any source distribution.
*/

int16_t rand(int16_t max) {
	max;
__asm
	ld hl, #(_rand_seed)
	ld a, (hl)
	add a, #0xB3
	ld (hl+), a
	adc a, (hl)
	ld (hl+), a
	adc a, (hl)
	ld (hl+), a
	ld d, a
	adc a, (hl)
	ld (hl), a
	ld e, a
	ldhl	sp,	#2
	ld	a, (hl+)
	ld	c, a
	ld	b, (hl)
	push	bc
	push	de
	call	__moduint
	add	sp, #4
__endasm;
}

int16_t rand_mask(int16_t max) __preserves_regs(d, e) {
	max;
__asm
	ld hl, #(_rand_seed)
	ld a, (hl)
	add a, #0xB3
	ld (hl+), a
	adc a, (hl)
	ld (hl+), a
	adc a, (hl)
	ld (hl+), a
	ld c, a
	adc a, (hl)
	ld (hl), a
	ld b, a
	ldhl sp, #2
	ld a, (hl+)
	and a, b
	ld b, a
	ld a, (hl)
	and a, c
	ld c, a
__endasm;
}
#endif

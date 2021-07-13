#include <stdlib.h>
#include "math.h"

// helper functions

int8_t signum8(int8_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

int8_t signum16(int16_t x) {
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

int16_t difference16(int16_t a, int16_t b) {
	return abs(a - b);
}

// RNG

#ifndef USE_XORSHIFT_RNG
static uint32_t rand_seed;
#else
static uint16_t rand_seed;
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
int16_t rand(int16_t max) {
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
#endif

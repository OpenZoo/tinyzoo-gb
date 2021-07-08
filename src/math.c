#include <stdlib.h>
#include "math.h"

#ifdef ENGINE_ACCURATE_RNG
static uint32_t rand_seed;
#else
static uint16_t rand_seed;
#endif

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

#ifdef ENGINE_ACCURATE_RNG
int16_t rand(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) % max;
}

int16_t rand_mask(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) & max;
}

void srand(uint16_t seed) {
	rand_seed = seed;
}
#else
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
	ld (_rand_seed), a
	xor a, h
	ld (_rand_seed+1), a
__endasm;
	return rand_seed % max;
}

int16_t rand_mask(int16_t max) {
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
	xor a, h
	ld (_rand_seed+1), a
__endasm;
	return rand_seed & max;
}

void srand(uint16_t seed) {
	rand_seed = seed == 0 ? 1 : seed;
}
#endif
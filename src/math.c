#include <stdlib.h>
#include "math.h"

static uint32_t rand_seed;

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

int16_t rand(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) % max;
}

void srand(uint32_t seed) {
	rand_seed = seed;
}
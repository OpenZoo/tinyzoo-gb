#include <stdlib.h>
#include "math.h"

static uint32_t rand_seed;

int16_t rand(int16_t max) {
	rand_seed = (rand_seed * 134775813) + 1;
	return (rand_seed >> 16) % max;
}

void srand(uint32_t seed) {
	rand_seed = seed;
}
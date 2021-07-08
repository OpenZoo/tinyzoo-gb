#ifndef __MATH_H__
#define __MATH_H__

#include <stdint.h>
#include <stdlib.h>

#define signum(x) _Generic((x), \
	int8_t: signum8, \
	default: signum16 \
)(x)

#define difference(a, b) _Generic(((a)+(b)), \
	int8_t: difference8, \
	default: difference16 \
)(x)

int8_t signum8(int8_t x);
int8_t signum16(int16_t x);
int8_t difference8(int8_t a, int8_t b);
int16_t difference16(int16_t a, int16_t b);

int16_t rand(int16_t max);
int16_t rand_mask(int16_t max);
void srand(uint16_t seed);

#define RAND2() rand_mask(1)

#endif /* __MATH_H__ */
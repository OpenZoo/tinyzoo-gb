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

static inline int8_t signum8(int8_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

static inline int8_t signum16(int16_t x) {
	if (x < 0) {
		return -1;
	} else if (x > 0) {
		return 1;
	} else {
		return 0;
	}
}

static inline int8_t difference8(int8_t a, int8_t b) {
	return abs(a - b);
}

static inline int16_t difference16(int16_t a, int16_t b) {
	return abs(a - b);
}

int16_t rand(int16_t max);
void srand(uint32_t seed);

#endif /* __MATH_H__ */
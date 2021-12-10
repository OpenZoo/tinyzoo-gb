#ifndef __CONFIG_H__
#define __CONFIG_H__

// Use accurate RNG. Very slow, but matches ZZT's original behaviour.
// #define USE_ACCURATE_RNG
// Use XorShift-based RNG by John Metcalf. Fast.
// #define USE_XORSHIFT_RNG
// Use RNG by Damian Yerrick. Faster.
#define USE_YERRICK_RNG

#define FEAT_BLACK_KEYS

#endif /* __CONFIG_H__ */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#define ROM_DATA_START_BANK 4
#define SRAM_MAX_BANK 16
#define MAX_BOARD 101

// Use accurate RNG. Very slow, but matches ZZT's original behaviour.
// #define USE_ACCURATE_RNG
// Use XorShift-based RNG by John Metcalf. Fast.
// #define USE_XORSHIFT_RNG
// Use RNG by Damian Yerrick. Faster.
#define USE_YERRICK_RNG

#define FEAT_BLACK_KEYS

// adds 81 bytes, performance untested
// #define OPT_UNROLL_LINE_DRAW

#define TORCH_DX 8
#define TORCH_DY 5
#define TORCH_DIST_SQR 50
#define MAX_FLAG 10
#define MAX_STAT 150
#define MAX_ELEMENT 53
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 25

#define MAX_DATA_OFS_SIZE 750
#define MAX_SOUND_BUFFER_SIZE 254

#endif /* __CONFIG_H__ */

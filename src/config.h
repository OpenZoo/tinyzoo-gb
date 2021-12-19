#ifndef __CONFIG_H__
#define __CONFIG_H__

#define ROM_DATA_START_BANK 4
#define MAX_BOARD 101
#define SRAM_MAX_BANK 16

// Use accurate RNG. Very slow, but matches ZZT's original behaviour.
// #define USE_ACCURATE_RNG
// Use XorShift-based RNG by John Metcalf. Fast.
// #define USE_XORSHIFT_RNG
// Use RNG by Damian Yerrick. Faster.
#define USE_YERRICK_RNG

// #define FEAT_BLACK_KEYS // Black keys/doors compat - costs ~235 bytes (as of 18/12/2021)
#define FEAT_BOARD_TRANSITIONS
#define BUGFIX_BLINKWALL_OFFSET
// #define BUGFIX_DIE_UNDER
#define BUGFIX_DIEMOVE_SCROLL
#define BUGFIX_DIEMOVE_OBJECT
#define BUGFIX_DIEMOVE_MESSAGE
#define BUGFIX_KOOPO_BUG
// #define BUGFIX_PUT_RANGE

// adds 81 bytes, performance untested
// #define OPT_UNROLL_LINE_DRAW

// saves 124 (!!!) bytes of RAM; however, loses 124 bytes of bank0 ROM
// this makes some parts of the board edge immutable
#define OPT_BOARD_EDGE_IMMUTABLE

#define TORCH_DX 8
#define TORCH_DY 5
#define TORCH_DIST_SQR 50
#define MAX_FLAG 10
#define MAX_STAT 150
#define MAX_ELEMENT 53
#define BOARD_WIDTH 60
#define BOARD_HEIGHT 25
#define MAX_OOP_INSTRUCTION_COUNT 32

#define MAX_DATA_OFS_SIZE 600
#define MAX_SOUND_BUFFER_SIZE 254
#define MAX_TEXT_WINDOW_LINES 512

#define SRAM_DEBUG
#ifdef SRAM_DEBUG
#define SRAM_DEBUG_OOP_EXECUTE
#endif

#endif /* __CONFIG_H__ */

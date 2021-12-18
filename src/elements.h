#ifndef __ELEMENTS_H__
#define __ELEMENTS_H__

#include <stdint.h>
#include "gamevars.h"

extern const uint8_t zoo_element_defs_character[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_color[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_flags[MAX_ELEMENT + 1]; // bank 0
extern const zoo_element_draw_proc zoo_element_defs_drawprocs[MAX_ELEMENT + 1]; // bank 1
extern const zoo_element_tick_proc zoo_element_defs_tickprocs[MAX_ELEMENT + 1]; // bank 1
extern const zoo_element_touch_proc zoo_element_defs_touchprocs[MAX_ELEMENT + 1]; // bank 1
extern const uint8_t zoo_element_defs_scorevalues[MAX_ELEMENT + 1]; // bank 0
extern const uint8_t zoo_element_defs_cycles[MAX_ELEMENT + 1]; // bank 2

#define E_EMPTY 0
#define E_BOARD_EDGE 1
#define E_MESSAGE_TIMER 2
#define E_MONITOR 3
#define E_PLAYER 4
#define E_AMMO 5
#define E_TORCH 6
#define E_GEM 7
#define E_KEY 8
#define E_DOOR 9
#define E_SCROLL 10
#define E_PASSAGE 11
#define E_DUPLICATOR 12
#define E_BOMB 13
#define E_ENERGIZER 14
#define E_STAR 15
#define E_CONVEYOR_CW 16
#define E_CONVEYOR_CCW 17
#define E_BULLET 18
#define E_WATER 19
#define E_FOREST 20
#define E_SOLID 21
#define E_NORMAL 22
#define E_BREAKABLE 23
#define E_BOULDER 24
#define E_SLIDER_NS 25
#define E_SLIDER_EW 26
#define E_FAKE 27
#define E_INVISIBLE 28
#define E_BLINK_WALL 29
#define E_TRANSPORTER 30
#define E_LINE 31
#define E_RICOCHET 32
#define E_BLINK_RAY_EW 33
#define E_BEAR 34
#define E_RUFFIAN 35
#define E_OBJECT 36
#define E_SLIME 37
#define E_SHARK 38
#define E_SPINNING_GUN 39
#define E_PUSHER 40
#define E_LION 41
#define E_TIGER 42
#define E_BLINK_RAY_NS 43
#define E_CENTIPEDE_HEAD 44
#define E_CENTIPEDE_SEGMENT 45
#define E_TEXT_BLUE 47
#define E_TEXT_GREEN 48
#define E_TEXT_CYAN 49
#define E_TEXT_RED 50
#define E_TEXT_PURPLE 51
#define E_TEXT_YELLOW 52
#define E_TEXT_WHITE 53

#define E_TEXT_MIN E_TEXT_BLUE

#endif

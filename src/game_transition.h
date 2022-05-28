#ifndef __GAME_TRANSITION_H__
#define __GAME_TRANSITION_H__

#include <stdbool.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include "gamevars.h"
#include "renderer.h"

#define GAME_TRANSITION_TO_BOARD 0xFF

void game_transition_start(uint8_t color);
void game_transition_step(void);
bool game_transition_running(void);
// void game_transition_tick_to_end(void) BANKED;

#define game_transition_board_change_start() game_transition_start(0x05)
#define game_transition_board_change_end() game_transition_start(GAME_TRANSITION_TO_BOARD)

#endif /* __GAME_TRANSITION_H__ */
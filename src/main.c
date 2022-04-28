#include <stdint.h>
#include <gb/gb.h>
#include "bank_switch.h"
#include "game.h"

#ifdef PROFILING
#include <gb/bgb_emu.h>
#endif

void init_main(void) BANKED;

void main(void) {
	ZOO_INIT_ROM;

	init_main();

	game_play_loop(true);

	while (1);
}

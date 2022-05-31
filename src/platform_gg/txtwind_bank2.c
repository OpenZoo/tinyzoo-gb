#pragma bank 2

#include <gbdk/platform.h>

#include "bank_switch.h"
#include "config.h"
#include "game.h"
#include "gamevars.h"
#include "input.h"
#include "oop.h"
#include "renderer.h"
#include "sram_alloc.h"
#include "txtwind.h"

bool txtwind_exec_line(uint16_t idx);

bool txtwind_run(void) BANKED {
	// TODO
}

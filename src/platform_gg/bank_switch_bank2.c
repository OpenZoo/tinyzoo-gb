#pragma bank 2

#include "bank_switch.h"

uint8_t zoo_get_ram_bank_count(void) BANKED {
	return 2;
}

#pragma bank 2

#include "bank_switch.h"

#ifdef __TPP1__
#else
static uint8_t mbc5_bank_counts[] = {0, 0, 1, 4, 16, 8};
#endif

uint8_t zoo_get_ram_bank_count(void) BANKED {
#ifdef __TPP1__
	return 1 << ((*(uint8_t*)0x0152)-1);
#else /* __MBC5__ */
	return mbc5_bank_counts[(*(uint8_t*)0x0149)];
#endif
}

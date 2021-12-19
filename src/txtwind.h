#ifndef __TXTWIND_H__
#define __TXTWIND_H__

#include <stdbool.h>
#include <stdint.h>
#include <gb/gb.h>
#include "gamevars.h"

extern uint16_t txtwind_lines;

// txtwind.c (bank 0)

// txtwind_bank2.c (bank 2)

void txtwind_init(void) BANKED;
void txtwind_append(uint16_t line_ptr, uint8_t line_bank) BANKED;
void txtwind_get_line(uint16_t idx, far_ptr_t *line_ptr) BANKED;

#endif /* __TXTWIND_H__ */
#ifndef __TXTWIND_H__
#define __TXTWIND_H__

#include <stdbool.h>
#include <stdint.h>
#include <gbdk/platform.h>
#include "gamevars.h"

#define TXTWIND_LINE_TYPE_REGULAR 0
#define TXTWIND_LINE_TYPE_CENTERED 1
#define TXTWIND_LINE_TYPE_HYPERLINK 2
#define MAX_TXTWIND_LINE_WIDTH 20
#define TXTWIND_LINE_HEADER_LEN 2
#define TXTWIND_LINE_AFTER_LEN 2

typedef struct {
	uint8_t type;
	uint8_t len;
	uint8_t text[MAX_TXTWIND_LINE_WIDTH + TXTWIND_LINE_AFTER_LEN];
} txtwind_line_t;

extern uint16_t txtwind_lines;

// txtwind.c (bank 0)

void txtwind_read_line(int16_t idx, txtwind_line_t *line);

// txtwind_bank2.c (bank 2)

void txtwind_init(void) BANKED;
void txtwind_append(uint16_t line_ptr, uint8_t line_bank) BANKED;
void txtwind_open_license(void) BANKED;
uint8_t txtwind_run(uint8_t render_mode) BANKED;

#endif /* __TXTWIND_H__ */

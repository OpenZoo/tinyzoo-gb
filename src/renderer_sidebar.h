#ifndef __RENDERER_SIDEBAR_H__
#define __RENDERER_SIDEBAR_H__

#include <stdint.h>
#include <stdbool.h>
#include <gbdk/platform.h>
#include "renderer.h"

void sidebar_set_message_color(uint8_t color) BANKED; // renderer_gbc_bank3.c

// renderer_sidebar_bank3.c (bank 3)

void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED;
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED;

// renderer_sidebar.c (bank 0)

void sidebar_show_message(const char* line1, uint8_t bank1, const char* line2, uint8_t bank2, const char* line3, uint8_t bank3);
void sidebar_hide_message(void);

#endif /* __RENDERER_SIDEBAR_H__ */

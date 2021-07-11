#ifndef __RENDERER_SIDEBAR_H__
#define __RENDERER_SIDEBAR_H__

#include "renderer.h"

void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED;
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED;

void sidebar_show_message(const char* line1, const char* line2, const char* line3) BANKED;
void sidebar_hide_message(void) BANKED;

void sidebar_set_message_color(uint8_t color) BANKED; // renderer_gbc_bank3.c

#endif /* __RENDERER_SIDEBAR_H__ */

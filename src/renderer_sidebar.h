#ifndef __RENDERER_SIDEBAR_H__
#define __RENDERER_SIDEBAR_H__

#include "renderer.h"

void sidebar_draw_panel(uint8_t x_shifted, uint8_t chr, uint8_t col, int16_t value, bool wide) BANKED;
void sidebar_draw_keys(uint8_t x_shifted, uint8_t value) BANKED;

#endif /* __RENDERER_SIDEBAR_H__ */

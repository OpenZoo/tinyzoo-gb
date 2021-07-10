#ifndef __ELEMENTS_UTILS_H__
#define __ELEMENTS_UTILS_H__

#include <stdint.h>
#include <stdbool.h>

void ElementMove(uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y);
void ElementTransporterMove(uint8_t x, uint8_t y, int8_t dx, int8_t dy);
void ElementPushablePush(uint8_t x, uint8_t y, int8_t dx, int8_t dy);
void DrawPlayerSurroundings(uint8_t x, uint8_t y, uint8_t bomb_phase);

#endif
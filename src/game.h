#ifndef __GAME_H__
#define __GAME_H__

#include <stdint.h>
#include <gb/gb.h>
#include "gamevars.h"

#define VIEWPORT_MIN_X 1
#define VIEWPORT_MAX_X (BOARD_WIDTH - VIEWPORT_WIDTH + 1)
#define VIEWPORT_MIN_Y 1
#define VIEWPORT_MAX_Y (BOARD_HEIGHT - VIEWPORT_HEIGHT + 1)

extern const zoo_stat_t stat_template_default;
extern const uint8_t line_tiles[16];
extern const int8_t neighbor_delta_x[4];
extern const int8_t neighbor_delta_y[4];
extern const int8_t diagonal_delta_x[8];
extern const int8_t diagonal_delta_y[8];

void center_viewport_on_player(void);
void board_redraw(void);

void board_create(void) BANKED; // 2
void world_create(void) BANKED; // 2
void board_enter(void);
void board_change(uint8_t id);
void board_draw_tile(uint8_t x, uint8_t y);
uint8_t get_stat_id_at(uint8_t x, uint8_t y);
void add_stat(uint8_t tx, uint8_t ty, uint8_t element, uint8_t color, uint8_t cycle, const zoo_stat_t *template);
void remove_stat(uint8_t stat_id);
void move_stat(uint8_t stat_id, uint8_t x, uint8_t y);
bool board_shoot(uint8_t element, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t source);
void damage_stat(uint8_t stat_id);
void board_damage_tile(uint8_t x, uint8_t y);
void board_attack(uint8_t stat_id, uint8_t x, uint8_t y);
void calc_direction_rnd(int8_t *dx, int8_t *dy);
void calc_direction_seek(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);
void game_play_loop(bool board_changed);
void game_update_sidebar(void);

#endif

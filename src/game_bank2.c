#pragma bank 2

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <gb/gb.h>
#include "board_manager.h"
#include "gamevars.h"
#include "game.h"
#include "elements.h"
#include "input.h"
#include "math.h"
#include "renderer.h"
#include "sound_consts.h"
#include "timer.h"

static const zoo_tile_t TileNormal = {E_NORMAL, 0x0E};
static const zoo_stat_t StatCreateMinusOne = {
	0, 1,
	0, 0, 0,
	0, 1, 0,
	1, 1,
	{1, 0}
};
static const zoo_stat_t StatCreatePlayer = {
	BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1,
	0, 0, 1,
	0, 0, 0,
	STAT_ID_NONE, STAT_ID_NONE,
	{0, 0}
};

extern uint8_t viewport_x;
extern uint8_t viewport_y;

void board_create(void) BANKED {
	memset(&zoo_board_info, 0, sizeof(zoo_board_info));
	memset(zoo_tiles, 0, sizeof(zoo_tile_t) * 64 * 27);
	
	zoo_board_info.max_shots = 255;

	for (uint8_t iy = 0; iy <= BOARD_HEIGHT + 1; iy++) {
		ZOO_TILE(0, iy).element = E_BOARD_EDGE;
		ZOO_TILE(BOARD_WIDTH + 1, iy).element = E_BOARD_EDGE;
	}
	for (uint8_t ix = 0; ix <= BOARD_WIDTH; ix++) {
		ZOO_TILE(ix, 0).element = E_BOARD_EDGE;
		ZOO_TILE(ix, BOARD_HEIGHT + 1).element = E_BOARD_EDGE;
	}
	for (uint8_t iy = 1; iy <= BOARD_HEIGHT; iy++) {
		ZOO_TILE_COPY(ZOO_TILE(1, iy), TileNormal);
		ZOO_TILE_COPY(ZOO_TILE(BOARD_WIDTH, iy), TileNormal);
	}
	for (uint8_t ix = 2; ix < BOARD_WIDTH; ix++) {
		ZOO_TILE_COPY(ZOO_TILE(ix, 1), TileNormal);
		ZOO_TILE_COPY(ZOO_TILE(ix, BOARD_HEIGHT), TileNormal);
	}

	zoo_stat_count = 0;
	ZOO_STAT(-1) = StatCreateMinusOne; 

	ZOO_TILE(BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1).element = E_PLAYER;
	ZOO_TILE(BOARD_WIDTH >> 1, BOARD_HEIGHT >> 1).color = 0x1F; // TODO

	ZOO_STAT(0) = StatCreatePlayer;
	center_viewport_on_player();
}

void world_create(void) BANKED {
	// TODO: boardcount, boardlen
	memset(&msg_flags, 0, sizeof(msg_flags));
	board_create();
	memset(&zoo_world_info, 0, sizeof(zoo_world_info));
	zoo_world_info.health = 100;
	memset(&zoo_world_info.flags, 255, sizeof(zoo_world_info.flags));
	// TODO: boardchange
}

void move_stat_scroll_stat0(uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y) {
	if ((zoo_board_info.flags & BOARD_IS_DARK) && (zoo_world_info.torch_ticks > 0)) {
		// TODO: optimize
		board_redraw();
	} else {
		// move viewport?
		int8_t pox = new_x - viewport_x;
		int8_t poy = new_y - viewport_y;
		int8_t dist = difference8(old_x, new_x) + difference8(old_y, new_y);

		if (dist > 1) {
			// full viewport adjust
			// TODO: optimize?
			uint8_t ov_x = viewport_x;
			uint8_t ov_y = viewport_y;
			center_viewport_on_player();
			int8_t vd_y = viewport_y - ov_y;
			if (viewport_x == ov_x) {
				text_scroll(0, vd_y);
				if (vd_y < 0) {
					for (uint8_t iy = 0; iy < (uint8_t) (-vd_y); iy++) {
						for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
							board_draw_tile(ix + viewport_x, iy + viewport_y);
						}
					}
				} else {
					for (uint8_t iy = 0; iy < (uint8_t) (vd_y); iy++) {
						for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
							board_draw_tile(ix + viewport_x, viewport_y + VIEWPORT_HEIGHT - 1 - iy);
						}
					}
				}
			} else {
				text_mark_redraw();
				board_redraw();
			}
		} else if (pox == VIEWPORT_PLAYER_MIN_X-1) {
			// move left
			if (viewport_x > VIEWPORT_MIN_X) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_undraw_tile(viewport_x + VIEWPORT_WIDTH - 1, iy + viewport_y);
				}
				viewport_x--;
				text_scroll(-1, 0);
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_draw_tile(viewport_x, iy + viewport_y);
				}
			}
		} else if (pox == VIEWPORT_PLAYER_MAX_X+1) {
			// move right
			if (viewport_x < VIEWPORT_MAX_X) {
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_undraw_tile(viewport_x, iy + viewport_y);
				}
				viewport_x++;
				text_scroll(1, 0);
				for (uint8_t iy = 0; iy < VIEWPORT_HEIGHT; iy++) {
					board_draw_tile(viewport_x + VIEWPORT_WIDTH - 1, iy + viewport_y);
				}
			}
		} else if (poy == VIEWPORT_PLAYER_MIN_Y-1) {
			// move up
			if (viewport_y > VIEWPORT_MIN_Y) {
				viewport_y--;
				text_scroll(0, -1);
				for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
					board_draw_tile(ix + viewport_x, viewport_y);
				}
			}
		} else if (poy == VIEWPORT_PLAYER_MAX_Y+1) {
			// move down
			if (viewport_y < VIEWPORT_MAX_Y) {
				viewport_y++;
				text_scroll(0, 1);
				for (uint8_t ix = 0; ix < VIEWPORT_WIDTH; ix++) {
					board_draw_tile(ix + viewport_x, viewport_y + VIEWPORT_HEIGHT - 1);
				}
			}
		}
	}
}

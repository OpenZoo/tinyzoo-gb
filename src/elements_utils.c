#pragma bank 1

#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "math.h"
#include "sound_consts.h"
#include "timer.h"

void ElementMove(uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y) {
	uint8_t stat_id = get_stat_id_at(old_x, old_y);
	if (stat_id != STAT_ID_NONE) {
		move_stat(stat_id, new_x, new_y);
	} else {
		ZOO_TILE_COPY(ZOO_TILE(new_x, new_y), ZOO_TILE(old_x, old_y));
		board_draw_tile(new_x, new_y);
		ZOO_TILE(old_x, old_y).element = E_EMPTY;
		board_draw_tile(old_x, old_y);
	}
}

void ElementPushablePush(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	ZOO_TILE_ASSIGN(tile, x, y);
	if (((tile.element == E_SLIDER_NS) && (dx == 0)) || ((tile.element == E_SLIDER_EW) && (dy == 0))
		|| (zoo_element_defs[tile.element].flags & ELEMENT_PUSHABLE)) {
		
		zoo_tile_t dtile;
		ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		if (dtile.element == E_TRANSPORTER) {
			ElementTransporterMove(x, y, dx, dy);
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		} else if (dtile.element != E_EMPTY) {
			if (dx != 0 || dy != 0) {
				ElementPushablePush(x + dx, y + dy, dx, dy);
				ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
			}
		}

		if (!(zoo_element_defs[dtile.element].flags & ELEMENT_WALKABLE)
			&& (zoo_element_defs[dtile.element].flags & ELEMENT_DESTRUCTIBLE)
			&& (tile.element != E_PLAYER))
		{
			board_damage_tile(x + dx, y + dy);
			ZOO_TILE_ASSIGN(dtile, x + dx, y + dy);
		}

		if (zoo_element_defs[dtile.element].flags & ELEMENT_WALKABLE) {
			ElementMove(x, y, x + dx, y + dy);
		}
	}
}

void ElementTransporterMove(uint8_t x, uint8_t y, int8_t dx, int8_t dy) {
	zoo_tile_t tile;
	zoo_stat_t *stat = &ZOO_STAT_AT(x + dx, y + dy);
	if ((stat->step_x == dx) && (stat->step_y == dy)) {
		uint8_t ix = x;
		uint8_t iy = y;
		bool is_valid_dest = true;
		while (1) {
			ix += dx;
			iy += dy;
			ZOO_TILE_ASSIGN(tile, ix, iy);
			if (tile.element == E_BOARD_EDGE) {
				break;
			} else if (is_valid_dest) {
				is_valid_dest = false;

				if (!(zoo_element_defs[tile.element].flags & ELEMENT_WALKABLE)) {
					ElementPushablePush(ix, iy, dx, dy);
					ZOO_TILE_ASSIGN(tile, ix, iy);
				}

				if (zoo_element_defs[tile.element].flags & ELEMENT_WALKABLE) {
					ElementMove(stat->x - dx, stat->y - dy, ix, iy);
					sound_queue(3, sound_transporter_move);
					break;
				}
			}

			if (tile.element == E_TRANSPORTER) {
				zoo_stat_t *stat2 = &ZOO_STAT_AT(ix, iy);
				if (stat2->step_x == -dx && stat2->step_y == -dy) {
					is_valid_dest = true;
				}
			}
		}
	}
}

void DrawPlayerSurroundings(uint8_t x, uint8_t y, uint8_t bomb_phase) {
	for (uint8_t ix = x - TORCH_DX - 1; ix <= x + TORCH_DX + 1; ix++) {
		if (ix < 1) continue;
		if (ix > BOARD_WIDTH) break;
		for (uint8_t iy = y - TORCH_DY - 1; iy <= y + TORCH_DY + 1; iy++) {
			if (iy < 1) continue;
			if (iy > BOARD_HEIGHT) break;

			zoo_tile_t *tile = &ZOO_TILE(ix, iy);
			if (bomb_phase > 0) {
				int16_t dx = ix - x;
				int16_t dy = iy - y;
				int16_t dist = (dx*dx) + (dy*dy) << 1;
				if (dist < TORCH_DIST_SQR) {
					if (bomb_phase == 1) {
						if (zoo_element_defs[tile->element].flags & ELEMENT_TYPICALLY_TEXTED) {
							uint8_t i_stat = get_stat_id_at(ix, iy);
							if (i_stat > 0) {
								// TODO OopSend
							}
						}

						if (tile->element == E_STAR || (zoo_element_defs[tile->element].flags & ELEMENT_DESTRUCTIBLE)) {
							board_damage_tile(ix, iy);
						}

						if (tile->element == E_EMPTY || tile->element == E_BREAKABLE) {
							tile->element = E_BREAKABLE;
							tile->color = 9 + rand(7);
						}
					} else {
						if (tile->element == E_BREAKABLE) {
							tile->element = E_EMPTY;
						}
					}
				}
			}
			board_draw_tile(ix, iy);
		}
	}
}
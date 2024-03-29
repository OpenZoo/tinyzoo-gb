#pragma bank 1

/**
 * Copyright (c) 2020, 2021, 2022 Adrian Siekierka
 *
 * TinyZoo/GB is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * TinyZoo/GB is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with TinyZoo/GB. If not, see <https://www.gnu.org/licenses/>. 
 *
 * TinyZoo/GB includes code derived from the Reconstruction of ZZT, used under
 * the following license terms:
 *
 * Copyright (c) 2020 Adrian Siekierka
 *
 * Based on a reconstruction of code from ZZT,
 * Copyright 1991 Epic MegaGames, used with permission.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../../res/message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

static int8_t player_dir_x, player_dir_y;

uint8_t ElementPlayerDraw(uint8_t x, uint8_t y) {
	return (zoo_world_info.energizer_ticks & 1) ? 0x01 : 0x02;
}

void ElementPlayerTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	uint8_t sx = stat->x;
	uint8_t sy = stat->y;

	zoo_tile_t *tile = &ZOO_TILE(sx, sy);

	if (zoo_world_info.energizer_ticks > 0) {
		if ((zoo_game_state.current_tick & 1) != 0) {
			tile->color = 0x0F;
		} else {
			tile->color = (zoo_modu16_8(zoo_game_state.current_tick, 7) << 4) + 0x1F;
		}

		board_draw_tile(sx, sy);
	} else if (tile->color != 0x1F) {
		tile->color = 0x1F;
		board_draw_tile(sx, sy);
	}

	if (zoo_world_info.health <= 0) {
		input_delta_x = 0;
		input_delta_y = 0;
		// gbzoo: replaced with a goto
		// input_shift_pressed = false;

		if (get_stat_id_at(0, 0) == STAT_ID_NONE) {
			display_message(255, NULL, msg_game_over_line1, msg_game_over_line2);
		}

		zoo_game_state.tick_time_duration = 0;
		sound_block_queueing = true;

		goto NoKeyInput;
	}

	if (input_shift_pressed) {
		if (input_delta_x != 0 || input_delta_y != 0) {
			player_dir_x = input_delta_x;
			player_dir_y = input_delta_y;
		} else {
			// gbzoo change: no SPACE key
			goto NoShootMove;
		}

		if (player_dir_x != 0 || player_dir_y != 0) {
			if (zoo_board_info.max_shots == 0) {
				if (!(msg_flags.f1 & MSG_FLAG1_NO_SHOOTING)) {
					display_message(200, NULL, msg_no_shooting_line1, msg_no_shooting_line2);
					msg_flags.f1 |= MSG_FLAG1_NO_SHOOTING;
				}
			} else if (zoo_world_info.ammo == 0) {
				if (!(msg_flags.f1 & MSG_FLAG1_OUT_OF_AMMO)) {
					display_message(200, NULL, msg_out_of_line1, msg_out_of_ammo_line2);
					msg_flags.f1 |= MSG_FLAG1_OUT_OF_AMMO;
				}
			} else {
				uint8_t bullet_count = 0;
				if (zoo_board_info.max_shots <= zoo_stat_count) {
					for (uint8_t i = 0; i <= zoo_stat_count; i++) {
						zoo_stat_t *bstat = &ZOO_STAT(i);
						if ((bstat->p1 == SHOT_SOURCE_PLAYER)
						&& (ZOO_TILE(bstat->x, bstat->y).element == E_BULLET)) {
							bullet_count++;
						}
					}
				}

				if (bullet_count < zoo_board_info.max_shots) {
					if (board_shoot(E_BULLET, sx, sy, player_dir_x, player_dir_y, SHOT_SOURCE_PLAYER)) {
						zoo_world_info.ammo--;
						game_update_sidebar_ammo();

						sound_queue(2, sound_player_shoot);

						input_delta_x = 0;
						input_delta_y = 0;
					}
				}
			}
		}
	} else if (input_delta_x != 0 || input_delta_y != 0) {
		player_dir_x = input_delta_x;
		player_dir_y = input_delta_y;

		zoo_element_defs_touchprocs[ZOO_TILE(sx + input_delta_x, sy + input_delta_y).element]
			(sx + input_delta_x, sy + input_delta_y, &input_delta_x, &input_delta_y);

		if (input_delta_x != 0 || input_delta_y != 0) {
			if (zoo_element_defs_flags[ZOO_TILE(sx + input_delta_x, sy + input_delta_y).element] & ELEMENT_WALKABLE) {
				move_stat(0, sx + input_delta_x, sy + input_delta_y);
			}
		}

		sx = stat->x;
		sy = stat->y;
	}

NoShootMove:
	if (input_b_pressed) {
		if (zoo_world_info.torch_ticks <= 0) {
			if (zoo_world_info.torches > 0) {
				if (zoo_board_info.flags & BOARD_IS_DARK) {
					zoo_world_info.torches--;
					zoo_world_info.torch_ticks = TORCH_DURATION;

					DrawPlayerSurroundings(sx, sy, 0);
					game_update_sidebar_torches();
				} else {
					if (!(msg_flags.f1 & MSG_FLAG1_ROOM_NOT_DARK)) {
						display_message(200, NULL, msg_room_not_dark_line1, msg_room_not_dark_line2);
						msg_flags.f1 |= MSG_FLAG1_ROOM_NOT_DARK;
					}
				}
			} else {
				if (!(msg_flags.f1 & MSG_FLAG1_OUT_OF_TORCHES)) {
					display_message(200, NULL, msg_out_of_line1, msg_out_of_torches_line2);
					msg_flags.f1 |= MSG_FLAG1_OUT_OF_TORCHES;
				}
			}
		}
	}

NoKeyInput:
	if (zoo_world_info.torch_ticks > 0) {
		if ((--zoo_world_info.torch_ticks) <= 0) {
			DrawPlayerSurroundings(sx, sy, 0);
			sound_queue(3, sound_torch_burnout);
		}

		// TODO
		/* if (zoo_modu16_8(zoo_world_info.torch_ticks, 40) == 0) {
			game_update_sidebar();
		} */
	}

	if (zoo_world_info.energizer_ticks > 0) {
		zoo_world_info.energizer_ticks--;

		if (zoo_world_info.energizer_ticks == 10) {
			sound_queue(9, sound_energizer_finish);
		} else if (zoo_world_info.energizer_ticks <= 0) {
			tile->color = 0x1F;
			board_draw_tile(sx, sy);
		}
	}

	if (zoo_board_info.time_limit_sec > 0 && zoo_world_info.health > 0) {
		if (timer_has_time_elapsed(&zoo_world_info.board_time_hsec, 100)) {
			zoo_world_info.board_time_sec++;
			game_update_sidebar_gems_time();

			if ((zoo_board_info.time_limit_sec - 10) == zoo_world_info.board_time_sec) {
				display_message(200, NULL, NULL, msg_out_of_time);
				sound_queue(3, sound_time_running_out);
			} else if (zoo_world_info.board_time_sec > zoo_board_info.time_limit_sec) {
				damage_stat(0);
			}
		}
	}
}

#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../input.h"
#include "../math.h"
#include "../message_consts.h"
#include "../sound_consts.h"
#include "../timer.h"

static int8_t player_dir_x, player_dir_y;

uint8_t ElementPlayerDraw(uint8_t x, uint8_t y) {
	return ((zoo_world_info.energizer_ticks > 0) && (zoo_world_info.energizer_ticks & 1))
		? 0x01 : 0x02;
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
			tile->color = ((zoo_game_state.current_tick % 7) << 4) + 0x1F;
		}

		board_draw_tile(sx, sy);
	} else if (tile->color != 0x1F) {
		tile->color = 0x1F;
		board_draw_tile(sx, sy);
	}

	if (zoo_world_info.health <= 0) {
		input_delta_x = 0;
		input_delta_y = 0;
		input_shift_pressed = false;

		if (get_stat_id_at(0, 0) == STAT_ID_NONE) {
			display_message(255, NULL, msg_game_over_line1, msg_game_over_line2);
		}

		zoo_game_state.tick_time_duration = 0;
		sound_block_queueing = true;
	}

	if (input_shift_pressed) {
		if (input_delta_x != 0 || input_delta_y != 0) {
			player_dir_x = input_delta_x;
			player_dir_y = input_delta_y;
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

		zoo_element_defs[ZOO_TILE(sx + input_delta_x, sy + input_delta_y).element]
			.touch_proc(sx + input_delta_x, sy + input_delta_y, &input_delta_x, &input_delta_y);

		if (input_delta_x != 0 || input_delta_y != 0) {
			if (zoo_element_defs[ZOO_TILE(sx + input_delta_x, sy + input_delta_y).element].flags & ELEMENT_WALKABLE) {
				move_stat(0, sx + input_delta_x, sy + input_delta_y);
			}
		}

		sx = stat->x;
		sy = stat->y;
	}
	
	// TODO

	if (zoo_world_info.torch_ticks > 0) {
		if ((--zoo_world_info.torch_ticks) <= 0) {
			DrawPlayerSurroundings(sx, sy, 0);
			sound_queue(3, sound_torch_burnout);
		}

		// TODO
		/* if ((zoo_world_info.torch_ticks % 40) == 0) {
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

			if ((zoo_board_info.time_limit_sec - 10) == zoo_world_info.board_time_sec) {
				display_message(200, NULL, NULL, msg_out_of_time);
				sound_queue(3, sound_time_running_out);
			} else if (zoo_world_info.board_time_sec > zoo_board_info.time_limit_sec) {
				damage_stat(0);
			}
		}
	}
}
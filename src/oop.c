#include <gb/gb.h>
#include <gbdk/emu_debug.h>

#include "bank_switch.h"
#include "elements.h"
#include "elements_utils.h"
#include "game.h"
#include "gamevars.h"
#include "config.h"
#include "math.h"
#include "oop.h"
#include "sram_alloc.h"
#include "timer.h"
#include "txtwind.h"

const char oop_object_name[] = "Interaction";
const char oop_scroll_name[] = "Scroll";

uint8_t oop_stat_id = 255;
static uint16_t oop_pos;
static zoo_stat_t *oop_stat;
static uint8_t *oop_prog_loc;
static uint8_t *oop_code_loc;
static uint8_t *oop_last_code_loc;
static bool oop_running_skippable;
static uint8_t oop_cmd;
static int8_t oop_dir_x;
static int8_t oop_dir_y;
static bool oop_stop_running;
static uint8_t oop_replace_element;
static uint8_t oop_replace_color;

static uint16_t find_label_loc;

#define SET_ZAP_NONE 255
#define SET_ZAP_FALSE 0
#define SET_ZAP_TRUE 1
#define SET_ZAP_FALSE_IF_FIND_STRING_VISIBLE 2

static void oop_command_end(void) {
	oop_pos = 0xFFFF;
	oop_stop_running = true;
}
#define oop_command_error oop_command_end

static void oop_before_potential_stat_list_change() {
	if (oop_pos != 0xFFFF) {
		oop_pos = oop_code_loc - (oop_prog_loc + 5);
	}
	oop_stat->data_pos = oop_pos;
}

static void oop_after_potential_stat_list_change() {
#ifdef BUGFIX_DIEMOVE_OOP_EXEC
	oop_stat = &ZOO_STAT(oop_stat_id);
#endif
	if (oop_stat->data_ofs == 0xFFFF) {
		oop_command_end();
	} else {
		uint8_t *data_loc = zoo_stat_data + oop_stat->data_ofs;
		ZOO_SWITCH_ROM(data_loc[2]);
		uint8_t *prog_loc = *((uint8_t**) data_loc);
		oop_pos = oop_stat->data_pos;
		oop_prog_loc = prog_loc;
		oop_code_loc = oop_prog_loc + 5 + oop_pos;
	}
}

bool oop_find_label_in_stat(uint8_t stat_id, uint8_t label_id, bool zapped, uint8_t set_zap) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->data_ofs == 0xFFFF) {
		return false;
	}

	if (label_id == OOP_LABEL_RESTART) {
		find_label_loc = 0;
		return true;
	}

	uint8_t prev_bank = _current_bank;

	uint8_t *data_loc = zoo_stat_data + stat->data_ofs;
	ZOO_SWITCH_ROM(data_loc[2]);
	uint8_t *prog_loc = *((uint8_t**) data_loc);

	uint16_t label_offset = *((uint16_t*) (prog_loc + 3));
	if (label_offset == 0) {
		goto FindLabelReturn;
	}

	uint8_t *label_loc = prog_loc + label_offset;
	uint8_t label_count = *(label_loc++);
	for (uint8_t i = 0; i < label_count; i++) {
		uint8_t label_id_at_loc = *(label_loc++);
		if (label_id_at_loc == label_id) {
			uint8_t *data_zap_loc = data_loc + 4 + (i >> 3);
			find_label_loc = (*((uint16_t*) label_loc));
			uint8_t zap_mask = (1 << (i & 7));
			bool find_label_zapped = ((*data_zap_loc) & zap_mask) != 0;
			if (find_label_zapped == zapped) {
				if (set_zap == SET_ZAP_FALSE) {
					(*data_zap_loc) &= ~zap_mask;
				} else if (set_zap == SET_ZAP_TRUE) {
					(*data_zap_loc) |= zap_mask;
				} else if (set_zap == SET_ZAP_FALSE_IF_FIND_STRING_VISIBLE) {
					if ((find_label_loc & 0x8000) == 0) goto FindLabelNotUsed;
					(*data_zap_loc) &= ~zap_mask;
				}

				ZOO_SWITCH_ROM(prev_bank);
				return true;
			}
		}

FindLabelNotUsed:
		// not jumping
		label_loc += 2;
	}

FindLabelReturn:
	ZOO_SWITCH_ROM(prev_bank);
	return false;
}

bool oop_send(uint8_t stat_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock) {
	if (oop_find_label_in_stat(stat_id, label_id, false, SET_ZAP_NONE)) {
		zoo_stat_t *stat = &ZOO_STAT(stat_id);

		if (stat->p2 != 0 && !ignore_lock) {
			if (respect_self_lock || stat_id != oop_stat_id) {
				return false;
			}
		}

		stat->data_pos = find_label_loc & 0x7FFF;
		return true;
	} else {
		return false;
	}
}

bool oop_send_target(uint8_t target_id, bool respect_self_lock, uint8_t label_id, bool ignore_lock) {
	if (label_id == OOP_LABEL_VOID || target_id == OOP_TARGET_VOID) {
		return false;
	} else if (target_id == OOP_TARGET_SELF || target_id == OOP_TARGET_EMPTY) {
		return oop_send(oop_stat_id, respect_self_lock, label_id, ignore_lock);
	} else {
		bool result = false;
		uint8_t prev_bank = _current_bank;

		zoo_stat_t *stat = &ZOO_STAT(0);
		uint8_t stat_id = 0;
		/* OTHERS = 253, ALL = 254 */
		if (target_id >= OOP_TARGET_OTHERS) {
			for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
				if (stat_id == oop_stat_id) continue;
				if (stat->data_ofs != 0xFFFF) {
					if (oop_send(stat_id, respect_self_lock, label_id, ignore_lock)) {
						result = true;
					}
				}
			}
		} else {
			for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
				if (stat->data_ofs != 0xFFFF) {
					uint8_t *data_loc = zoo_stat_data + stat->data_ofs;
					ZOO_SWITCH_ROM(data_loc[2]);
					uint8_t *prog_loc = *((uint8_t**) data_loc);
					if (target_id == prog_loc[0]) {
						if (oop_send(stat_id, respect_self_lock, label_id, ignore_lock)) {
							result = true;
						}
					}
				}
			}
		}

		ZOO_SWITCH_ROM(prev_bank);
		return result;
	}
}

typedef void (*oop_zap_proc)(uint8_t, uint8_t, bool);

void oop_zap(uint8_t stat_id, uint8_t label_id, bool target_empty) {
	oop_find_label_in_stat(stat_id, label_id, false, SET_ZAP_TRUE);
}

void oop_restore(uint8_t stat_id, uint8_t label_id, bool target_empty) {
	// TODO: Optimize inner loop?
	if (oop_find_label_in_stat(stat_id, label_id, true, SET_ZAP_FALSE)) {
		if (target_empty) {
			while (oop_find_label_in_stat(stat_id, label_id, true, SET_ZAP_FALSE_IF_FIND_STRING_VISIBLE)) {
				// pass
			}
		}
	}
}

void oop_zap_target(uint8_t target_id, uint8_t label_id, oop_zap_proc zap_proc) {
	if (label_id == OOP_LABEL_VOID || target_id == OOP_TARGET_VOID) {
		return;
	} else if (target_id == OOP_TARGET_SELF) {
		zap_proc(oop_stat_id, label_id, false);
	} else if (target_id == OOP_TARGET_EMPTY) {
		zap_proc(oop_stat_id, label_id, true);
	} else {
		uint8_t prev_bank = _current_bank;

		zoo_stat_t *stat = &ZOO_STAT(0);
		uint8_t stat_id = 0;
		for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
			if (stat->data_ofs != 0xFFFF) {
				if (target_id == OOP_TARGET_ALL || (target_id == OOP_TARGET_OTHERS && stat_id != oop_stat_id)) {
					zap_proc(stat_id, label_id, false);
				} else {
					uint8_t *data_loc = zoo_stat_data + stat->data_ofs;
					ZOO_SWITCH_ROM(data_loc[2]);
					uint8_t *prog_loc = *((uint8_t**) data_loc);
					if (target_id == prog_loc[0]) {
						zap_proc(stat_id, label_id, false);
					}
				}
			}
		}

		ZOO_SWITCH_ROM(prev_bank);
	}
}

static void oop_parse_direction(void) {
	int8_t tmp;
	switch (*(oop_code_loc++)) {
		case 0x00:
		default:
			oop_dir_x = 0;
			oop_dir_y = 0;
			break;
		case 0x01:
			oop_dir_x = 0;
			oop_dir_y = -1;
			break;
		case 0x02:
			oop_dir_x = 0;
			oop_dir_y = 1;
			break;
		case 0x03:
			oop_dir_x = 1;
			oop_dir_y = 0;
			break;
		case 0x04:
			oop_dir_x = -1;
			oop_dir_y = 0;
			break;
		case 0x05: /* SEEK */
			calc_direction_seek(oop_stat->x, oop_stat->y, &oop_dir_x, &oop_dir_y);
			break;
		case 0x06: /* FLOW */
			oop_dir_x = oop_stat->step_x;
			oop_dir_y = oop_stat->step_y;
			break;
		case 0x07: /* RND */
			calc_direction_rnd(&oop_dir_x, &oop_dir_y);
			break;
		case 0x08: /* RNDNS */
			oop_dir_x = 0;
			oop_dir_y = (RAND2() * 2 - 1);
			break;
		case 0x09: /* RNDNE */
			oop_dir_x = RAND2();
			oop_dir_y = (oop_dir_x == 0) ? -1 : 0;
			break;
		case 0x0A: /* CW */
			oop_parse_direction();
			tmp = oop_dir_x;
			oop_dir_x = -oop_dir_y;
			oop_dir_y = tmp;
			break;
		case 0x0B: /* CCW */
			oop_parse_direction();
			tmp = oop_dir_x;
			oop_dir_x = oop_dir_y;
			oop_dir_y = -tmp;
			break;
		case 0x0C: /* RNDP */
			oop_parse_direction();
			if (RAND2() == 0) {
				oop_dir_x = -oop_dir_x;
			} else {
				oop_dir_y = -oop_dir_y;
			}
			break;
		case 0x0D: /* OPP */
			oop_parse_direction();
			oop_dir_x = -oop_dir_x;
			oop_dir_y = -oop_dir_y;
			break;
	}
}

static bool oop_check_condition(void) {
	uint8_t i, j, x, y;

	switch (*(oop_code_loc++)) {
		case 0x00: /* NOT */
			return !oop_check_condition();
		case 0x01: /* ALLIGNED */
			return (oop_stat->x == ZOO_STAT(0).x) || (oop_stat->y == ZOO_STAT(0).y);
		case 0x02: /* CONTACT */
			return (abs(oop_stat->x - ZOO_STAT(0).x) + abs(oop_stat->y - ZOO_STAT(0).y)) == 1;
		case 0x03: /* BLOCKED */
			oop_parse_direction();
			x = oop_stat->x + oop_dir_x;
			y = oop_stat->y + oop_dir_y;
			return !(zoo_element_defs_flags[ZOO_TILE(x, y).element] & ELEMENT_WALKABLE);
		case 0x04: /* ENERGIZED */
			return zoo_world_info.energizer_ticks != 0;
		case 0x05: /* ANY */
			i = *(oop_code_loc++);
			j = *(oop_code_loc++);
			x = 0;
			y = 1;
			return find_tile_on_board(&x, &y, i, j);
		case 0x06: /* FLAG */
			i = *(oop_code_loc++);
			return i != FLAG_ID_NONE && world_get_flag_pos(i) != FLAG_ID_NONE;
	}

	return false;
}

static inline void oop_skip_command(void) {
	uint8_t len = *(oop_code_loc++);
	oop_code_loc += len;
}

static inline void oop_run_skippable_command(void) {
	oop_code_loc++;
	oop_running_skippable = true;
}

typedef void (*oop_command_proc)(void);

static void oop_command_direction(void) {
	oop_stop_running = true;

	oop_parse_direction();
	if (oop_cmd >= 0x03 || oop_dir_x != 0 || oop_dir_y != 0) {
		uint8_t dest_x = oop_stat->x + oop_dir_x;
		uint8_t dest_y = oop_stat->y + oop_dir_y;
		if (ZOO_TILE_READBOUNDS(dest_x, dest_y)) {
			oop_before_potential_stat_list_change();
			if (!(zoo_element_defs_flags[ZOO_TILE(dest_x, dest_y).element] & ELEMENT_WALKABLE)) {
				uint8_t prev_bank = _current_bank;
				ZOO_SWITCH_ROM(1);
				ElementPushablePush(dest_x, dest_y, oop_dir_x, oop_dir_y);
				ZOO_SWITCH_ROM(prev_bank);
				oop_after_potential_stat_list_change();
			} else {
				// minor optimization
				goto OopDirMoveStat;
			}
			if (zoo_element_defs_flags[ZOO_TILE(dest_x, dest_y).element] & ELEMENT_WALKABLE) {
OopDirMoveStat:
				move_stat(oop_stat_id, dest_x, dest_y);
				oop_after_potential_stat_list_change();
				if (oop_cmd == 0x04) {
					oop_skip_command();
				}
				return;
			}
		}
	} else {
		return;
	}

	if (oop_cmd == 0x01 /* /dir */ || oop_cmd == 0x03 /* GO */) {
		oop_code_loc = oop_last_code_loc;
	} else if (oop_cmd == 0x04 /* TRY */) {
		oop_run_skippable_command();
		oop_stop_running = false;
	}
}

static void oop_command_walk(void) {
	oop_parse_direction();
	oop_stat->step_x = oop_dir_x;
	oop_stat->step_y = oop_dir_y;
}

static void oop_command_set(void) {
	world_set_flag(*(oop_code_loc++));
}

static void oop_command_clear(void) {
	world_clear_flag(*(oop_code_loc++));
}

static void oop_command_if(void) {
	if (oop_check_condition()) {
		oop_run_skippable_command();
	} else {
		oop_skip_command();
	}
}

static void oop_command_noop(void) {

}

static void oop_command_shoot(void) {
	oop_parse_direction();
	board_shoot(oop_cmd == 0x0A ? E_STAR : E_BULLET, oop_stat->x, oop_stat->y, oop_dir_x, oop_dir_y, SHOT_SOURCE_ENEMY);
	oop_stop_running = true;
}

static void oop_command_endgame(void) {
	zoo_world_info.health = 0;
}

static void oop_command_idle(void) {
	oop_stop_running = true;
}

static void oop_command_restart(void) {
	oop_code_loc = oop_prog_loc + 5;
}

static void oop_command_zap(void) {
	uint8_t target_id = *(oop_code_loc++);
	uint8_t label_id = *(oop_code_loc++);
	oop_zap_target(target_id, label_id, oop_zap);
}

static void oop_command_restore(void) {
	uint8_t target_id = *(oop_code_loc++);
	uint8_t label_id = *(oop_code_loc++);
	oop_zap_target(target_id, label_id, oop_restore);
}

static void oop_command_send(void) {
	uint8_t target_id = *(oop_code_loc++);
	uint8_t label_id = *(oop_code_loc++);

	// update before oop_send
	if (oop_pos != 0xFFFF) {
		oop_pos = oop_code_loc - (oop_prog_loc + 5);
	}
	oop_stat->data_pos = oop_pos;

	oop_send_target(target_id, false, label_id, false);

	// update after oop_send
	oop_pos = oop_stat->data_pos;
	oop_code_loc = oop_prog_loc + 5 + oop_pos;
}

static int16_t *oop_give_ptrs[] = {
	&zoo_world_info.health,
	&zoo_world_info.ammo,
	&zoo_world_info.gems,
	&zoo_world_info.torches,
	&zoo_world_info.score,
	&zoo_world_info.board_time_sec
};

static oop_command_proc oop_give_procs[] = {
	game_update_sidebar_health,
	game_update_sidebar_ammo,
	game_update_sidebar_gems_time,
	game_update_sidebar_torches,
	game_update_sidebar_score,
	game_update_sidebar_gems_time
};

static void oop_command_give(void) {
	uint8_t ptr_id = *(oop_code_loc++);
	int16_t *ptr = oop_give_ptrs[ptr_id];
	int16_t val = *((int16_t*) oop_code_loc);
	oop_code_loc += 2;
	val += *ptr;
	if (val >= 0) {
		*ptr = val;
		oop_give_procs[ptr_id]();
		oop_skip_command();
	} else {
		oop_run_skippable_command();
	}
}

static void oop_command_lock(void) {
	oop_stat->p2 = (oop_cmd == 0x12) ? 1 : 0;
}

static void oop_command_put(void) {
	oop_parse_direction();
	uint8_t element = *(oop_code_loc++);
	uint8_t color = *(oop_code_loc++);
	if (oop_dir_x == 0 && oop_dir_y == 0) {
		oop_command_error();
		return;
	}

	int8_t nx = oop_stat->x + oop_dir_x;
	int8_t ny = oop_stat->y + oop_dir_y;

#ifdef BUGFIX_PUT_RANGE
	if (nx > 0 && ny > 0 && nx <= BOARD_WIDTH && ny <= BOARD_HEIGHT) {
#else
	if (nx > 0 && ny > 0 && nx <= BOARD_WIDTH && ny < BOARD_HEIGHT) {
#endif
		oop_before_potential_stat_list_change();
		if (!(zoo_element_defs_flags[ZOO_TILE(nx, ny).element] & ELEMENT_WALKABLE)) {
			uint8_t prev_bank = _current_bank;
			ZOO_SWITCH_ROM(1);
			ElementPushablePush(nx, ny, oop_dir_x, oop_dir_y);
			ZOO_SWITCH_ROM(prev_bank);
		}

		oop_place_tile(nx, ny, element, color);
		oop_after_potential_stat_list_change();
	}
}

static void oop_command_change(void) {
	uint8_t element_from = *(oop_code_loc++);
	uint8_t color_from = *(oop_code_loc++);
	uint8_t element_to = *(oop_code_loc++);
	uint8_t color_to = *(oop_code_loc++);
	uint8_t ix = 0;
	uint8_t iy = 1;
	bool changed = false;
	if (color_to == 0) {
		color_to = zoo_element_defs_color[element_to];
		if (color_to >= COLOR_SPECIAL_MIN) {
			color_to = 0;
		}
	}
	oop_before_potential_stat_list_change();
	while (find_tile_on_board(&ix, &iy, element_from, color_from)) {
		oop_place_tile(ix, iy, element_to, color_to);
		changed = true;
	}
	if (changed) {
		oop_after_potential_stat_list_change();
	}
}

static void oop_command_become(void) {
	oop_replace_element = *(oop_code_loc++);
	oop_replace_color = *(oop_code_loc++);
	oop_stop_running = true;
}

static void oop_command_play(void) {
	sound_queue_nobank(-1, oop_code_loc);
	oop_code_loc += (*oop_code_loc) + 1;
}

static void oop_command_cycle(void) {
	// the cycle range check is moved to the converter
	oop_stat->cycle = *(oop_code_loc++);
}

static void oop_command_char(void) {
	// the char range check is moved to the converter
	oop_stat->p1 = *(oop_code_loc++);
	board_draw_tile(oop_stat->x, oop_stat->y);
}

static void oop_command_die(void) {
#ifdef BUGFIX_DIE_UNDER
	oop_replace_element = oop_stat->under.element;
	oop_replace_color = oop_stat->under.color;
#else
	oop_replace_element = E_EMPTY;
	oop_replace_color = 0x0F;
#endif
	oop_stop_running = true;
}

static void oop_command_bind(void) {
	uint8_t target_id = *(oop_code_loc++);
	uint8_t prev_bank = _current_bank;

	zoo_stat_t *stat = &ZOO_STAT(0);
	uint8_t stat_id = 0;
	for (; stat_id <= zoo_stat_count; stat_id++, stat++) {
		if (stat->data_ofs != 0xFFFF) {
			uint8_t *data_loc = zoo_stat_data + stat->data_ofs;
			ZOO_SWITCH_ROM(data_loc[2]);
			uint8_t *prog_loc = *((uint8_t**) data_loc);
			if (target_id == prog_loc[0]) {
				if (oop_stat_id == stat_id) break;

				// #BIND does not clone DataOfs, but rather assigns it
				oop_dataofs_free_if_unused(oop_stat->data_ofs, oop_stat_id);
				oop_stat->data_ofs = stat->data_ofs;
				// oop_stat->data_pos = 0; (done later anyway)

				// update cached values
				oop_pos = 0;
				oop_prog_loc = prog_loc;
				oop_code_loc = oop_prog_loc + 5 + oop_pos;

				return; // we DO want to change bank here
			}
		}
	}

	ZOO_SWITCH_ROM(prev_bank);
}

uint16_t oop_window_zzt_lines;

static void oop_command_text_line(void) {
	uint8_t zzt_line_count = *(oop_code_loc++);
	uint8_t line_count = *(oop_code_loc++);
	if (zzt_line_count == 0) {
		if (oop_window_zzt_lines == 0) {
			// skip empty line
			oop_code_loc += (line_count * 3);
			return;
		} else {
			zzt_line_count = 1;
		}
	}
	if (oop_window_zzt_lines == 0) {
		// init text window
		txtwind_init();
	}
	oop_window_zzt_lines += zzt_line_count;

	while ((line_count--) > 0) {
		txtwind_append(*((uint16_t*) oop_code_loc), oop_code_loc[2]);
		oop_code_loc += 3;
	}
}

static oop_command_proc oop_procs[] = {
	oop_command_end,
	oop_command_direction, // /
	oop_command_direction, // ?
	oop_command_direction, // GO
	oop_command_direction, // TRY
	oop_command_walk,
	oop_command_set,
	oop_command_clear,
	oop_command_if,
	oop_command_shoot, // SHOOT
	oop_command_shoot, // THROWSTAR
	oop_command_give,
	oop_command_noop,
	oop_command_endgame,
	oop_command_idle,
	oop_command_restart,
	oop_command_zap,
	oop_command_restore,
	oop_command_lock, // LOCK
	oop_command_lock, // UNLOCK
	oop_command_send,
	oop_command_become,
	oop_command_put,
	oop_command_change,
	oop_command_play,
	oop_command_cycle,
	oop_command_char,
	oop_command_die,
	oop_command_bind,
	oop_command_text_line,
};

static uint8_t oop_ins_cost[] = {
	1, // #END
	0, // /
	0, // ?
	1, // #GO
	1, // #TRY
	1, // #WALK
	1, // #SET
	1, // #CLEAR
	1, // #IF
	1, // #SHOOT
	1, // #THROWSTAR
	1, // #GIVE
	1, // no-op/replacement
	1, // #ENDGAME
	1, // #IDLE
	1, // #RESTART
	1, // #ZAP
	1, // #RESTORE
	1, // #LOCK
	1, // #UNLOCK
	1, // #SEND
	1, // #BECOME
	1, // #PUT
	1, // #CHANGE
	1, // #PLAY
	1, // #CYCLE
	1, // #CHAR
	1, // #DIE
	1, // #BIND
	0, // text line
};

static uint8_t ins_count;

bool oop_handle_txtwind(void) BANKED OLDCALL;

bool oop_execute(uint8_t stat_id, const char *name) OLDCALL {
	uint8_t prev_bank = _current_bank;

	oop_stat_id = stat_id;
	oop_stat = &ZOO_STAT(oop_stat_id);

OopStartParsing:
	if (oop_stat->data_ofs == 0xFFFF) {
		return false;
	}

	oop_pos = oop_stat->data_pos;
	uint8_t *oop_data_loc = zoo_stat_data + oop_stat->data_ofs;

	ZOO_SWITCH_ROM(oop_data_loc[2]);

#ifdef DEBUG_PRINTFS_OOP_EXEC
	EMU_printf("executing stat %u @ %d,%d %x:%x, pos %u", stat_id, oop_stat->x, oop_stat->y, oop_data_loc[2], *((uint16_t*)oop_data_loc), oop_pos);
#endif

	oop_prog_loc = *((uint8_t**) oop_data_loc);
	oop_code_loc = oop_prog_loc + 5 + oop_pos;
	oop_stop_running = false;
	oop_replace_element = 255;
	ins_count = MAX_OOP_INSTRUCTION_COUNT;
	oop_window_zzt_lines = 0;

	while (ins_count > 0 && !oop_stop_running) {
		if (oop_running_skippable) {
			oop_running_skippable = false;
		} else {
			oop_last_code_loc = oop_code_loc;
		}
		oop_cmd = *(oop_code_loc++);
#ifdef DEBUG_PRINTFS_OOP_EXEC
		if (oop_pos != 0xFFFF) {
			oop_pos = oop_code_loc - (oop_prog_loc + 5);
		}
		EMU_printf("- pos %d, cmd 0x%x", oop_pos, oop_cmd);
#endif
		ins_count -= oop_ins_cost[oop_cmd];
		oop_procs[oop_cmd]();
	}

	if (oop_pos != 0xFFFF) {
		oop_pos = oop_code_loc - (oop_prog_loc + 5);
	}

	oop_stat->data_pos = oop_pos;

	ZOO_SWITCH_ROM(prev_bank);

	if (oop_window_zzt_lines != 0) {
		if (oop_handle_txtwind()) {
			goto OopStartParsing;
		}
	}

	if (oop_replace_element != 255) {
		uint8_t ix = oop_stat->x;
		uint8_t iy = oop_stat->y;
		damage_stat(oop_stat_id);
		oop_place_tile(ix, iy, oop_replace_element, oop_replace_color);
		oop_stat_id = STAT_ID_NONE;
		return true;
	} else {
		oop_stat_id = STAT_ID_NONE;
		return false;
	}
}

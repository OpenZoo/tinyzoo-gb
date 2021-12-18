#include "config.h"
#include "oop.h"

// TODO: Good candidate for GBZ80 optimization
uint8_t world_get_flag_pos(uint8_t flag_id) {
	for (uint8_t i = 0; i < MAX_FLAG; i++) {
		if (zoo_world_info.oop_flags[i] == flag_id) {
			return i;
		}
	}
	return FLAG_ID_NONE;
}

void world_set_flag(uint8_t flag_id) {
	if (world_get_flag_pos(flag_id) == FLAG_ID_NONE) {
		for (uint8_t i = 0; i < MAX_FLAG; i++) {
			if (zoo_world_info.oop_flags[i] == FLAG_ID_NONE) {
				zoo_world_info.oop_flags[i] = flag_id;
				return;
			}
		}
	}
}

void world_clear_flag(uint8_t flag_id) {
	uint8_t pos = world_get_flag_pos(flag_id);
	if (pos != FLAG_ID_NONE) {
		zoo_world_info.oop_flags[pos] = FLAG_ID_NONE;
	}
}
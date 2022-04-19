#pragma bank 1

#include <gbdk/emu_debug.h>
#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../renderer_sidebar.h"
#include "../sound_consts.h"
#include "../timer.h"

void ElementMessageTimerTick(uint8_t stat_id) {
	zoo_stat_t *stat = &ZOO_STAT(stat_id);
	if (stat->x == 0) {
		sidebar_set_message_color(9 + (stat->p2 % 7));
		if ((--(stat->p2)) == 0) {
			remove_stat(stat_id);
			zoo_game_state.current_stat_ticked--;
			sidebar_hide_message();
		}
	}
}

#include <string.h>
#include "gamevars.h"
#include "game.h"
#include "board_manager.h"

void load_board_data(const uint8_t *data) {
	// board length
	data += 2;

	// RLE decode
	uint8_t ix = 1;
	uint8_t iy = 1;
	uint8_t rle_count = 0;
	zoo_tile_t rle_tile;
	do {
			if (rle_count <= 0) {
					rle_count = *(data++);
					rle_tile.element = *(data++);
					rle_tile.color = *(data++);
			}
			ZOO_TILE_COPY(ZOO_TILE(ix, iy), rle_tile);
			ix++;
			if (ix > BOARD_WIDTH) {
					ix = 1;
					iy++;
			}
			rle_count--;
	} while (iy <= BOARD_HEIGHT);

	memcpy(&zoo_board_info, data, sizeof(zoo_board_info_t));
	data += sizeof(zoo_board_info_t);

	zoo_stat_count = *(data++);
	memcpy(zoo_stats + 1, data, sizeof(zoo_stat_t) * (zoo_stat_count + 1));

	center_viewport_on_player();
}
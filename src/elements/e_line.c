#pragma bank 1

#include "../elements.h"
#include "../elements_utils.h"
#include "../gamevars.h"
#include "../game.h"
#include "../math.h"
#include "../sound_consts.h"
#include "../timer.h"

static const uint8_t line_tiles[16] = {
	249, 208, 210, 186,
	181, 188, 187, 185,
	198, 200, 201, 204,
	205, 202, 203, 206
};

uint8_t ElementLineDraw(uint8_t x, uint8_t y) {
	uint8_t v = 0;
#ifdef OPT_UNROLL_LINE_DRAW
	uint8_t element = ZOO_TILE(x, y - 1).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 1;
	element = ZOO_TILE(x, y + 1).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 2;
	element = ZOO_TILE(x - 1, y).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 4;
	element = ZOO_TILE(x + 1, y).element;
	if (element == E_LINE || element == E_BOARD_EDGE) v |= 8;
#else
	uint8_t i, shift = 1;
	for (i = 0; i < 4; i++, shift <<= 1) {
		uint8_t element = ZOO_TILE(x + neighbor_delta_x[i], y + neighbor_delta_y[i]).element;
		if (element == E_LINE || element == E_BOARD_EDGE) {
			v |= shift;
		}
	}
#endif
	return line_tiles[v];
}

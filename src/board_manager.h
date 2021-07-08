#ifndef __BOARD_MANAGER_H__
#define __BOARD_MANAGER_H__

void load_world(uint16_t offset) NONBANKED;
void load_board(uint8_t offset) NONBANKED;
void load_board_data(uint8_t bank, const uint8_t *data) NONBANKED;

#endif /* __BOARD_MANAGER_H__ */
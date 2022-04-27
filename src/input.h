#ifndef __INPUT_H__
#define __INPUT_H__

#include <stdbool.h>
#include <stdint.h>
#include <gb/gb.h>

extern int8_t input_delta_x, input_delta_y;
extern uint8_t input_held;

#define input_a_pressed ((input_held & J_A) != 0)
#define input_b_pressed ((input_held & J_B) != 0)
#define input_select_pressed ((input_held & J_SELECT) != 0)
#define input_start_pressed ((input_held & J_START) != 0)
#define input_shift_pressed input_a_pressed

void input_update(void);
void input_update_vbl(void);

#endif /* __INPUT_H__ */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include <stdint.h>
#include <gb/gb.h>

extern bool sound_block_queueing;

void sound_init(void) BANKED;
void sound_clear_queue(void) BANKED;
void sound_queue(int8_t priority, const uint8_t *data) BANKED;
void sound_queue_nobank(int8_t priority, const uint8_t *data);

void timer_init(void);
uint16_t timer_hsecs(void);
bool timer_has_time_elapsed(uint16_t *ctr, uint16_t duration);

#endif /* __TIMER_H__ */
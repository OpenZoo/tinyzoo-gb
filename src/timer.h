#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include <stdint.h>

void sound_queue(int8_t priority, const uint8_t *data);
void timer_init(void);
uint16_t timer_hsecs(void);
bool timer_has_time_elapsed(uint16_t *ctr, uint16_t duration);

#endif /* __TIMER_H__ */
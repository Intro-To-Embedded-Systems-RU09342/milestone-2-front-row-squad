#ifndef FAN_H
#define FAN_H

#include <msp430.h>
#include <stdint.h>

void fan_init(void);
void fan_set(uint8_t speed);

#endif


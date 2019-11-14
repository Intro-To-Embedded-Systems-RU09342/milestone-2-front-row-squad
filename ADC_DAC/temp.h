#ifndef TEMP_H
#define TEMP_H

#include <msp430.h>
#include <stdint.h>

void temp_init(void);
uint8_t temp_get(uint16_t);
uint16_t temp_adc_get(void);

#endif


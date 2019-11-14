#include "fan.h"
#include "uart.h"

void fan_init()
{
}

void fan_set(uint8_t speed)
{
    uart_tx(0x11);
}


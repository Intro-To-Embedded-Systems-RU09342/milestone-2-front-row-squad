/*
 * Temperature-regualting fan system
 *
 * Records temperature using thermistor and controls a fan using PWM to acheive a specified temperature.
 *
 * Created: 11/7/19
 * Last Edited: 12/1/19
 * Author: Andrew Hollabaugh
 */

#include "main.h"
#include "temp.h"
#include "uart.h"
#include "fan.h"

int main()
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    uart_init();
    temp_init();
    fan_init();

    __bis_SR_register(GIE); //enable interrupts

    while(1)
    {
        uart_rx_check_queue(); //check the uart_rx queue for new commands

        //transmit formatted temperature and fan speed over uart
        uart_tx_str("t: ");
        uart_tx_num((uint16_t) temp_get());
        uart_tx_str(" f: ");
        uart_tx_num(fan_get_speed());
        uart_tx('\n');
    }
}


/*
 * Description
 *
 * Created: 11/7/19
 * Last Edited: 11/24/19
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

    __bis_SR_register(GIE); //enter low power mode and enable interrupts

    while(1)
    {
        uart_rx_check_queue();
        uart_tx_num((int) temp_get());
    }
}

//only for test
void set_temp(uint8_t temp)
{
    uart_tx_num(temp);
}


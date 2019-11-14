/*
 * Description
 *
 * Created: 11/7/19
 * Last Edited: 11/7/19
 * Author: Andrew Hollabaugh
 */

#include "main.h"
#include "temp.h"
#include "uart.h"
#include "fan.h"

uint8_t set_point = 0;
uint8_t temp = 10;

int main()
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    uart_init();
    //temp_init();

    __bis_SR_register(GIE); //enter low power mode and enable interrupts

    while(1)
    {
        //uart_tx(temp_adc_get());
        //uart_tx_str("t: ");
        //uart_tx(temp);
        //uart_tx_str("\n");
        uart_rx_check_queue();
    }
}

void set_temp(uint8_t temp)
{
    set_point = temp;
    uart_tx(0x22);
}


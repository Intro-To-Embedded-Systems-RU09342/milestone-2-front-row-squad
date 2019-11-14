#include "uart.h"
#include "rx_queue.h"
#include "fan.h"
#include "main.h"

#define CMD_BUFFER_LENGTH 5
#define SET_TEMP_CMD 'T'
#define SET_FAN_CMD 'F'

char cmd[CMD_BUFFER_LENGTH];
uint8_t cmd_len = 0;

void uart_init()
{
    DCOCTL = 0; //select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ; //set DC0 to 1 MHz
    DCOCTL = CALDCO_1MHZ;
    P1SEL |= BIT1 + BIT2; //P1.1 = RXD, P1.2 = TXD
    P1SEL2 |= BIT1 + BIT2; //P1.1 = RXD, P1.2 = TXD
    UCA0CTL1 |= UCSSEL_2; //SMCLK
    UCA0BR0 = 104; //set baud to 9600 from 1 MHz
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0; //modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST; //initialize USCI state machine
    IE2 |= UCA0RXIE; //enable USCI_A0 RX interrupt
}

void uart_tx(char data)
{
    while(!(IFG2 & UCA0TXIFG)); //wait until USCI_A0 TX buffer is ready
    UCA0TXBUF = data; //put byte in transmission buffer
}

void uart_tx_str(char* data)
{
    uint8_t len = sizeof(data) / sizeof(data[0]);
    for(uint8_t i = 0; i < len; i++)
    {
        uart_tx(data[i]);
    }
}

void uart_rx_check_queue()
{
    if(!rx_queue_is_empty())
        uart_rx_add_char_to_cmd(rx_queue_pop());
}

void uart_rx_add_char_to_cmd(char c)
{
    if(c != '\n') //add normal char
    {
        //if(cmd_len >= CMD_BUFFER_LENGTH)
            //uart_tx(0xff);
        //else
        cmd[cmd_len++] = c;
    }
    else if(cmd_len > 0) //newline is received; terminate command and execute
    {
        if(cmd_len == 1)
            uart_rx_execute_cmd();
        else
            uart_rx_execute_cmd_with_arg();
        cmd_len = 0;

        //memset(&cmd[0], 0, sizeof(cmd)); //clear cmd array
    }
}

//no commands without arguments, so useless for now
void uart_rx_execute_cmd()
{
    uart_tx(0xfa);
}

void uart_rx_execute_cmd_with_arg()
{
    uint16_t arg = 0;
    uint8_t arg_len = cmd_len - 1;

    //Does place value math on each arg char, and adds to arg int
    for(uint8_t i = 1; i <= arg_len; i++)
    {
        uint8_t exp_factor = arg_len - i;
        uint16_t mul_factor = 1;
        for(uint8_t j = 0; j < exp_factor; j++)
            mul_factor *= 10;
        arg += (cmd[i]-'0') * mul_factor;
    }

    if(cmd[0] == SET_TEMP_CMD)
        set_temp(arg);
    else if(cmd[0] == SET_FAN_CMD)
        fan_set(arg);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(!rx_queue_is_full())
        rx_queue_push(UCA0RXBUF); //push the rx'ed char to the rx_queue
    //uart_tx(0x10);
    //while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    //UCA0TXBUF = UCA0RXBUF;
}


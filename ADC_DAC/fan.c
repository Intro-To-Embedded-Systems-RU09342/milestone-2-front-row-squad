#include "fan.h"
#include "uart.h"
#include "temp.h"

#define CONTROL_LOOP_PERIOD 50000
#define MAX_OUTPUT 10000
#define MIN_OUTPUT 0
#define FAN_KICK_SPEED 4000
#define FAN_MIN_SPEED 1000

uint16_t target = 0;
uint16_t output = 0;
int64_t kp = -10;
double ki = -0.1;
int64_t i_term = 0;
uint8_t control_mode = 0; //manual = 0, auto = 1
uint8_t fan_kick_i = 0;

void fan_init()
{
    //timer A0, for fan output, P1.6
    P1DIR |= BIT6;
    P1SEL1 |= BIT6;
    P1SEL0 |= BIT6;
    TA0CTL |= TASSEL_2 + MC_1; //smclk, up mode
    TA0CCTL1 |= OUTMOD_7;
    TA0CCR0 = MAX_OUTPUT - 1;
    TA0CCR1 = 0;

    //timer A1, for control loop timing
    TA1CTL |= TASSEL_2 + MC_2 + ID_3; //smclk, continuous mode
    TA1CCTL0 = CCIE; //enable CCR0 interrupt
    TA1CCR0 = CONTROL_LOOP_PERIOD;
}

void fan_set(uint16_t speed)
{
    if(speed <= 0)
    {
        TA0CCTL1 &= ~OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
        output = MIN_OUTPUT;
    }
    else if(speed >= MAX_OUTPUT)
    {
        //uart_tx_num(speed);
        TA0CCTL1 |= OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
        output = MAX_OUTPUT;
    }
    else
    {
        TA0CCTL1 |= OUTMOD_7;
        TA0CCR1 = speed;
        output = speed;
    }
}

void fan_set_manual(uint16_t speed)
{
    control_mode = 0;
    fan_set(speed);
}

void fan_set_auto(uint16_t temp)
{
    control_mode = 1;
    target = temp;
}

uint16_t fan_get_speed()
{
    return output;
}

void fan_calc_speed()
{
    if(control_mode == 1)
    {
        int64_t current_value = temp_get();
        int64_t error = target - current_value;

        int64_t p_term = kp * error;
        i_term += (int64_t)(ki * error);

        //cap the integral to prevent windup
        if(i_term > MAX_OUTPUT)
            i_term = MAX_OUTPUT;
        else if(i_term < MIN_OUTPUT)
            i_term = MIN_OUTPUT;

        int64_t new_output = p_term + i_term;

        if(new_output > MAX_OUTPUT)
            new_output = MAX_OUTPUT;
        else if(new_output < MIN_OUTPUT)
            new_output = MIN_OUTPUT;

        if(error < 0 && new_output < FAN_MIN_SPEED)
            new_output = FAN_MIN_SPEED;

/*        
        if(fan_kick_i > 0)
        {
            new_output = FAN_KICK_SPEED;
            fan_kick_i--;
        }
        else if(new_output > FAN_MIN_SPEED && new_output < FAN_KICK_SPEED && output <= FAN_MIN_SPEED)
        {
            new_output = FAN_KICK_SPEED;
            fan_kick_i = 1;
        }*/
        
        fan_set((uint16_t) new_output);
    }
}

void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
{
    fan_calc_speed();
}

#include "temp.h"
#include "uart.h"

#define TEMP_PIN 0 //P1.0, A0
#define MAX_ADC_VAL 1023
#define MAX_VOLTAGE 3.3

uint16_t adc_val = 0;

void temp_init()
{
    P1SEL |= (1 << TEMP_PIN); 
    ADC10CTL0 |= ADC10SHT_2;
    ADC10CTL0 |= ADC10ON; //enable ADC10
    ADC10CTL0 |= ADC10IE; //enable ADC10 interrupt
    ADC10CTL1 |= TEMP_PIN;
    ADC10AE0 = 0x02; //set P1.1 to use ADC
}

uint8_t temp_get(uint16_t adc_val)
{
    double voltage = adc_val / MAX_ADC_VAL * MAX_VOLTAGE;
}

uint16_t temp_adc_get()
{
    return adc_val;
}
/*
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    //uart_tx(100);
    //adc_val = ADC10MEM;
    //temp_get(adc_val);
}
*/

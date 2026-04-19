#include "interrupt.h"

volatile uint16_t adc_buf[ADC_NUM_CHANNEL - 1], adcInter;
volatile uint8_t adc_conversion_complete, adc_overrun_count;

uint16_t GlobalTickCount, adc_value[ADC_NUM_CHANNEL - 1];


int main(void)
{
    RCC_Init();
    GPIO_Init();
    SysTick_Init();
    /*Здесь важен порядок инициализации, так как АЦП должен быть настроен до DMA,
     *а DMA до таймера, который запускает преобразование
     */
    ADC_Init();
    DMA_Init();
    TIM_PWM_Init();

    while (1)
    {

    }
}
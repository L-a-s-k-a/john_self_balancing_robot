#include "interrupt.h"

volatile uint16_t adc_buf[ADC_NUM_CHANNEL - 1], adcInter, motor_freq;
volatile uint8_t adc_conversion_complete, adc_overrun_count, motor_direction, permis = 1;

uint16_t GlobalTickCount, adc_value[ADC_NUM_CHANNEL - 1];
volatile float Temperature, Vsense;

void filter_testing(uint8_t permission);

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
        // if (adc_conversion_complete)
        // {
        //     adc_conversion_complete = 0; // Сброс флага готовности данных
        //     adc_value[0] = adc_buf[0];
        // }

        // voltage = adc_value[0] * ADC_DIVISION;
        // filter_volt += (voltage - filter_volt) * k_volt;

        filter_testing(permis);
        if (permis)
        {
            TIM3->CCR2 = 1;
        }
    }
}

void filter_testing(uint8_t permission)
{
    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BS6); // Задаём начальное направление вращения в одну сторону
    for (uint16_t time = 1; time <= 4500; time += 500)
    {
        if (permission)
        {
            return;
        }
        TIM3->CCR2 = time;
        delay(500);
    }
    for (uint16_t time = 4500; time >= 1; time -= 500)
    {
        if(permission){
            return;
        }
        TIM3->CCR2 = time;
        delay(500);
    }

    SET_BIT(GPIOA->BSRR, GPIO_BSRR_BR6); // Задаём направление вращения в другую сторону
    for (uint16_t time = 1; time <= 4500; time += 500)
    {
        if(permission){
            return;
        }
        TIM3->CCR2 = time;
        delay(500);
    }
    for (uint16_t time = 4500; time >= 1; time -= 500)
    {
        if(permission){
            return;
        }
        TIM3->CCR2 = time;
        delay(500);
    }
}

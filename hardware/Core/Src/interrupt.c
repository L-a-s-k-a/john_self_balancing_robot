#include "interrupt.h"

uint16_t tick_delay_count, miiliseconds, tim3_count;
uint8_t FLAG_Delay;
volatile float voltage, filter_volt = 0, k_volt = 0.01;

void SysTick_Handler(void)
{
    GlobalTickCount++;
    if (FLAG_Delay)
        tick_delay_count++;
}

void ADC_IRQHandler(void){
    adcInter = ADC1->DR; 
    // SET_BIT(ADC1->CR2, ADC_CR2_SWSTART);
}

void DMA2_Stream0_IRQHandler(void){
    if (READ_BIT(DMA2->LISR, DMA_LISR_TCIF0))
    {
        SET_BIT(DMA2->LIFCR, DMA_LIFCR_CTCIF0); // Очистка флага прерывания
        adc_conversion_complete = 1; // Установка флага готовности данных
    }
    // Диагностика: проверка переполнения АЦП
    if (ADC1->SR & ADC_SR_OVR) {
        adc_overrun_count++;
        ADC1->SR &= ~ADC_SR_OVR; // Сбрасываем флаг
    }
}

void TIM3_IRQHandler(void){
    if (READ_BIT(TIM3->SR, TIM_SR_UIF))
    {
        TIM3->SR &= ~TIM_SR_UIF; // Очистка флага прерывания
        voltage = adc_buf[0] * ADC_DIVISION;
        filter_volt += (voltage - filter_volt) * k_volt;
        current_from_voltage = (voltage - 1.484) / 0.05;           // 1.484 is a mean reference voltage value
        current_from_filter_volt = (filter_volt - 1.484) / 0.05;   // 0.05 is the shunt resistance value
        tim3_count++;
        if(tim3_count >= 1000){
            tim3_count = 0;
            miiliseconds++;
        }
    }
}

void delay(int del)
{
    FLAG_Delay = 1;
    while (tick_delay_count < del)
    {
    }
    tick_delay_count = 0;
    FLAG_Delay = 0;
}

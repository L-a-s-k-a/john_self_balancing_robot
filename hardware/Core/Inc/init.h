#pragma once
#ifndef INIT_H
#define INIT_H

#include "../../CMSIS/Devices/STM32F4xx/Inc/stm32f4xx.h"
#include "../../CMSIS/Devices/STM32F4xx/Inc/STM32F411xE/stm32f411xe.h"

#define ADC_NUM_CHANNEL 4

// Буфер для одного значения АЦП (16 бит)
extern volatile uint16_t adc_buf[ADC_NUM_CHANNEL - 1];
// Флаг готовности данных (устанавливается в прерывании DMA)
extern volatile uint8_t adc_conversion_complete, adc_overrun_count;

void RCC_Init(void);
void SysTick_Init(void);
void GPIO_Init(void);
void TIM_PWM_Init(void);
void ADC_Init(void);
void DMA_Init(void);
void I2C_Init(void);

#endif

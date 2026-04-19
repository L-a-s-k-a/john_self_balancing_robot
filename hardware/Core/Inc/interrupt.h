#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "init.h"

#define ADC_DIVISION (2.979f / 4095.0f)

extern volatile uint16_t adcInter;
extern uint16_t GlobalTickCount;
extern volatile float voltage, filter_volt, k_volt, current_from_voltage, current_from_filter_volt;

void EXTI15_10_IRQHandler(void);
void SysTick_Handler(void);
void TIM3_IRQHandler(void);
void TIM2_IRQHandler(void);
void delay(int del);

#endif
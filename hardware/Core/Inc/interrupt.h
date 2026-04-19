#pragma once
#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "init.h"

extern volatile uint16_t adcInter;
extern uint16_t GlobalTickCount;

void SysTick_Handler(void);
void ADC_IRQHandler(void);
void DMA2_Stream0_IRQHandler(void);
void TIM3_IRQHandler(void);
void delay(int del);

#endif
#include "interrupt.h"
#include "i2c.h"
#include "lsm303dlhc.h"

volatile uint16_t adc_buf[ADC_NUM_CHANNEL - 1], adcInter;
volatile uint8_t adc_conversion_complete, adc_overrun_count;

uint16_t GlobalTickCount, adc_value[ADC_NUM_CHANNEL - 1];

uint8_t data_accel[6], data_magnetic[6], dat;
int16_t accel[3], magnetic[3];
int16_t x_accel, y_accel, z_accel;
float x_roll, y_pitch, z_yaw, x_north, y_east, z_down;
float x_magnetic, y_magnetic, z_magnetic;

int main(void)
{
    RCC_Init();
    GPIO_Init();
    SysTick_Init();

    ADC_Init();
    DMA_Init();
    TIM_PWM_Init();
    I2C_Init();

    while (1)
    {   

    }
}
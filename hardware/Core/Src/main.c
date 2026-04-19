#include "interrupt.h"
#include "i2c.h"

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
    /*Здесь важен порядок инициализации, так как АЦП должен быть настроен до DMA,
     *а DMA до таймера, который запускает преобразование
     */
    ADC_Init();
    DMA_Init();
    TIM_PWM_Init();
    I2C_Init();

    /*--------Акселерометр--------*/
    I2C_WriteByte(0x19, 0x20, 0x57); // Активируем акселерометр (режим normal (100 Гц), все оси включены)
    I2C_WriteByte(0x19, 0x23, 0x08); // Устанавливаем режим выхода высокого разрешения и полный масштаб ±2g (FS = 00)
    /*--------Магнитометр--------*/
    I2C_WriteByte(0x1e, 0x00, 0x10); // Устанавливаем минимальную скорость вывода данных на 15 Гц (DO = 100)
    I2C_WriteByte(0x1e, 0x01, 0x20); // Устанавливаем усилитель на ±1.3 Гаусса (GN = 001)
    I2C_WriteByte(0x1e, 0x02, 0x00); // Устанавливаем режим непрерывного измерения (MD = 00)

    while (1)
    {   
        I2C_ReadByte(0x19, 0x20, &dat);
        I2C_ReadByte(0x19, 0x28, &data_accel[0]); // Ось Х, младший байт
        I2C_ReadByte(0x19, 0x29, &data_accel[1]); // Ось Х, старший байт
        I2C_ReadByte(0x19, 0x2A, &data_accel[2]); // Ось Y, младший байт
        I2C_ReadByte(0x19, 0x2B, &data_accel[3]); // Ось Y, старший байт
        I2C_ReadByte(0x19, 0x2C, &data_accel[4]); // Ось Z, младший байт
        I2C_ReadByte(0x19, 0x2D, &data_accel[5]); // Ось Z, старший байт
        accel[0] = (int16_t)((data_accel[1] << 8) | data_accel[0]); // Комбинирование старшего и младшего байта для оси X
        accel[1] = (int16_t)((data_accel[3] << 8) | data_accel[2]); // Комбинирование старшего и младшего байта для оси Y
        accel[2] = (int16_t)((data_accel[5] << 8) | data_accel[4]); // Комбинирование старшего и младшего байта для оси Z
        x_accel = accel[0] >> 4; // Сдвиг на 4 бита для получения значения в формате 12 бит
        y_accel = accel[1] >> 4; // Сдвиг на 4 бита для получения значения в формате 12 бит
        z_accel = accel[2] >> 4; // Сдвиг на 4 бита для получения значения в формате 12 бит
        x_roll = x_accel * 0.001f * 9.81f; // Поскольку мы используем режим ±2g, то 1 LSB соответствует примерно 0.001g, что эквивалентно 0.001 * 9.81 м/с²
        y_pitch = y_accel * 0.001f * 9.81f; // Дальше умножаем на 9.81 для получения значения в м/с², что может быть полезно для дальнейших расчётов
        z_yaw = z_accel * 0.001f * 9.81f;

        I2C_ReadByte(0x1e, 0x03, &data_magnetic[0]);
        I2C_ReadByte(0x1e, 0x04, &data_magnetic[1]);
        I2C_ReadByte(0x1e, 0x05, &data_magnetic[2]);
        I2C_ReadByte(0x1e, 0x06, &data_magnetic[3]);
        I2C_ReadByte(0x1e, 0x07, &data_magnetic[4]);
        I2C_ReadByte(0x1e, 0x08, &data_magnetic[5]);
        magnetic[0] = (int16_t)((data_magnetic[0] << 8) | data_magnetic[1]);
        magnetic[1] = (int16_t)((data_magnetic[2] << 8) | data_magnetic[3]);
        magnetic[2] = (int16_t)((data_magnetic[4] << 8) | data_magnetic[5]);
        x_magnetic = magnetic[0] / 1100.0f; // Преобразование в Гауссы (для ±1.3 Гаусса, 1100 LSB/Гаусс)
        y_magnetic = magnetic[1] / 1100.0f;
        z_magnetic = magnetic[2] / 980.0f;
        x_north = x_magnetic * 1000.0f; // Преобразование в мГауссы для удобства
        y_east = y_magnetic * 1000.0f;
        z_down = z_magnetic * 1000.0f;
    }
}
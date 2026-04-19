#include "i2c.h"

/**
  * @brief  Запись одного байта в регистр.
  * @param  slave_address:     Адрес устройства (например, 0x32)
  * @param  register_address:  Адрес регистра для записи (например, 0x0F)
  * @param  byte:              Записываемый байт
  * @retval None
  */
void I2C_WriteByte(uint8_t slave_address, uint8_t register_address, uint8_t byte){
    // Ждём, пока шина не освободится
    while(I2C1->SR2 & I2C_SR2_BUSY);

    // Генерация стартового условия
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));   // Ожидание установки флага SB

    // Отправка адреса устройства с битом записи (0)
    I2C1->DR = (slave_address << 1) | 0;     // Сдвиг адреса влево и установка бита записи
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Ожидание установки флага ADDR
    (void)I2C1->SR2;                     // Чтение SR2 для сброса флага ADDR

    // Отправка адреса регистра
    I2C1->DR = register_address;
    while (!(I2C1->SR1 & I2C_SR1_TXE));  // Ожидание установки флага TXE

    // Отправка данных
    I2C1->DR = byte;
    while (!(I2C1->SR1 & I2C_SR1_TXE));  // Ожидание установки флага TXE
    while (!(I2C1->SR1 & I2C_SR1_BTF));  // Ожидание завершения передачи
    
    // Генерация стопового условия
    I2C1->CR1 |= I2C_CR1_STOP;
}

/**
  * @brief  Чтение одного байта из регистра датчика.
  * @param  slave_address: Адрес датчика (например, 0x32)
  * @param  register_address:  Адрес регистра для чтения
  * @param  data:   Прочитанный байт
  */
 void I2C_ReadByte(uint8_t slave_address, uint8_t register_address, uint8_t *data){
    // Ждём, пока шина не освободится
    while(I2C1->SR2 & I2C_SR2_BUSY);

    // Генерация стартового условия
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));   // Ожидание установки флага SB

    // Отправка адреса устройства с битом записи (0)
    I2C1->DR = (slave_address << 1) | 0;      // Сдвигать адрес влево не надо, а установка бита записи нужна
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Ожидание установки флага ADDR
    (void)I2C1->SR2;                     // Чтение SR2 для сброса флага ADDR

    // Отправка адреса регистра
    I2C1->DR = register_address;
    while (!(I2C1->SR1 & I2C_SR1_TXE));  // Ожидание установки флага TXE
    while (!(I2C1->SR1 & I2C_SR1_BTF));  // Ожидание завершения передачи

    // Генерация повторного стартового условия для чтения
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));   // Ожидание установки флага SB

    // Отправка адреса устройства с битом чтения (1)
    I2C1->DR = (slave_address << 1) | 1; // Сдвиг адреса влево и установка бита чтения
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); // Ожидание установки флага ADDR
    (void)I2C1->SR2;                     // Чтение SR2 для сброса флага ADDR

    // Ожидание получения данных
    I2C1->CR1 &= ~I2C_CR1_ACK; // Отключение подтверждения для последнего байта
    while (!(I2C1->SR1 & I2C_SR1_RXNE));  // Ожидание установки флага RXNE

    *data = I2C1->DR;                     // Чтение данных из регистра DR

    // Генерация стопового условия
    I2C1->CR1 |= I2C_CR1_STOP;
    I2C1->CR1 |= I2C_CR1_ACK;  // вернуть ACK для будущих операций
 }
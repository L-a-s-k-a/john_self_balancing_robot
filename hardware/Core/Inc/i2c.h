#pragma once
#ifndef I2C_H
#define I2C_H

#include "init.h"

void I2C_WriteByte(uint8_t slave_address, uint8_t register_address, uint8_t data);
void I2C_ReadByte(uint8_t slave_address, uint8_t register_address, uint8_t *data);

#endif // I2C_H
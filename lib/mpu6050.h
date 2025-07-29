#ifndef MPU6050_H
#define MPU6050_H

#include <stdio.h>
#include <string.h>
#include <math.h>                    // Para as funções trigonométricas
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// Definição dos pinos I2C para o MPU6050
#define I2C_PORT i2c0                 // I2C0 usa pinos 0 e 1
#define I2C_SDA 0
#define I2C_SCL 1

// Endereço padrão do MPU6050
static int addr = 0x68;

void mpu6050_reset();
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);
void init_mpu6050();


#endif
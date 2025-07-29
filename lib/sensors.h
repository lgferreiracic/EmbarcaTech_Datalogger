#ifndef SENSORS_H
#define SENSORS_H

#include "ssd1306.h"
#include "mpu6050.h"
#include "hardware/i2c.h"
#include <math.h>

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

typedef struct{
    float acel_x;
    float acel_y;
    float acel_z;
    float giro_x;
    float giro_y;
    float giro_z;
}SensorReadings;

void init_i2c_sensor();
SensorReadings get_sensor_readings();

#endif

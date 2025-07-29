#include "sensors.h"


void init_i2c_sensor() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}


SensorReadings get_sensor_readings() {
    SensorReadings data;
    int16_t acc[3], gyro[3], temp;  //Variaveis para armazenar os dados
    mpu6050_read_raw(acc, gyro, &temp);
    data.acel_x = acc[0] / 16384.0f;
    data.acel_y = acc[1] / 16384.0f;
    data.acel_z = acc[2] / 16384.0f;
    data.giro_x = gyro[0] / 131.0f;
    data.giro_y = gyro[1] / 131.0f;
    data.giro_z = gyro[2] / 131.0f;
    return data;
}
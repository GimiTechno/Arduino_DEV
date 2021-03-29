#ifndef _BME280_I2C_H_
#define _BME280_I2C_H_

#include <stdio.h>
#include <stdint.h>

#include <Arduino.h>
#include <Wire.h>

typedef struct{
    /*< Compensated temperature */
    double temperature;

    /*< Compensated pressure */
    double pressure;

    /*< Compensated humidity */
    double humidity;
} bme280_data_t;

void bme280_i2C_init(void);
void bme280_i2c_main(bme280_data_t *p_bme280_data_t);

#endif /* _BME280_I2C_H_ */
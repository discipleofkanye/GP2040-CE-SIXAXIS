/* *****************************************************************************
 * Filename: Mpu6050.h
 *
 * Description: This is file contains the definition of the Mpu6050 class, that
 * is used to configure and read data from the eponymous sensor.
 * Each instance of this class represents a different sensor that has an I2C
 * address 0x68 or 0x69 if the AD0 pin is connected to logic 1 (3.3V).
 * The Mpu6050Data struct is also defined here and has 3 member variables for
 * each type of measurement (accel,gyro,temp).
 * A set of useful enumerations is defined for configuring the sensor in a more
 * readable way.
 *
 * Author: Thomas Havy
 *
 * License: MIT License - Copyright (c) 2017 Thomas Havy
 * A copy of the license can be found at:
 * https://github.com/Th-Havy/Simple-MPU6050-Arduino/blob/master/LICENSE
 *
 * Changes:
 * - Created: 07-Sept-2017
 *
 * ************************************************************************* */

/* Ported for GP2040 by HoloPengin 2023 */

#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>
#include "hardware/i2c.h"

// MPU6050 I2C address (default AD0 low)
#define MPU6050_ADDR 0x68

// Register map
#define MPU6050_WHO_AM_I     0x75
#define MPU6050_PWR_MGMT_1   0x6B
#define MPU6050_GYRO_CONFIG  0x1B
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_GYRO_ZOUT_H  0x47

class MPU6050 {
public:
    MPU6050();
    bool begin(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin);
    bool readRawAccel(int16_t &ax, int16_t &ay, int16_t &az);
    bool readRawGyro(int16_t &gx, int16_t &gy, int16_t &gz);

private:
    i2c_inst_t *_i2c;
    bool readRegister(uint8_t reg, uint8_t *data, uint8_t len);
    bool writeRegister(uint8_t reg, uint8_t value);
};

#endif

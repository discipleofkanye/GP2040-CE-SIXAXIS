/* *****************************************************************************
 * Filename: Mpu6050.cpp
 *
 * Description: This is file contains the implementation of the Mpu6050 class.
 * All the information you need to understand how to communicate and setup the
 * chip can be found in the datasheet and register map from Invensense:
 * https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
 * https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
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

#include "MPU6050.h"
#include "hardware/i2c.h"

MPU6050::MPU6050() : _i2c(nullptr) {}

bool MPU6050::begin(i2c_inst_t *i2c, uint8_t sda_pin, uint8_t scl_pin) {
    _i2c = i2c;
    // Initialize I2C (if not already done – caller must have already called i2c_init)
    // We'll assume the caller (i2cmpu6050.cpp) sets up I2C before calling this.
    
    // Wake up MPU6050
    if (!writeRegister(MPU6050_PWR_MGMT_1, 0x00))
        return false;
    
    // Wait for wake
    sleep_ms(10);
    
    // Verify WHO_AM_I
    uint8_t whoami = 0;
    if (!readRegister(MPU6050_WHO_AM_I, &whoami, 1))
        return false;
    if (whoami != 0x68)
        return false;
    
    // Set gyro range to ±250°/s (0x00)
    if (!writeRegister(MPU6050_GYRO_CONFIG, 0x00))
        return false;
    // Set accelerometer range to ±2g (0x00)
    if (!writeRegister(MPU6050_ACCEL_CONFIG, 0x00))
        return false;
    
    return true;
}

bool MPU6050::readRawAccel(int16_t &ax, int16_t &ay, int16_t &az) {
    uint8_t buf[6];
    if (!readRegister(MPU6050_ACCEL_XOUT_H, buf, 6))
        return false;
    ax = (int16_t)((buf[0] << 8) | buf[1]);
    ay = (int16_t)((buf[2] << 8) | buf[3]);
    az = (int16_t)((buf[4] << 8) | buf[5]);
    return true;
}

bool MPU6050::readRawGyro(int16_t &gx, int16_t &gy, int16_t &gz) {
    uint8_t buf[6];
    // Gyro registers start at 0x43 (we need Z only, but read all for completeness)
    if (!readRegister(0x43, buf, 6))
        return false;
    gx = (int16_t)((buf[0] << 8) | buf[1]);
    gy = (int16_t)((buf[2] << 8) | buf[3]);
    gz = (int16_t)((buf[4] << 8) | buf[5]);
    return true;
}

bool MPU6050::readRegister(uint8_t reg, uint8_t *data, uint8_t len) {
    if (!_i2c) return false;
    int ret = i2c_write_blocking(_i2c, MPU6050_ADDR, &reg, 1, true);
    if (ret != 1) return false;
    ret = i2c_read_blocking(_i2c, MPU6050_ADDR, data, len, false);
    return (ret == len);
}

bool MPU6050::writeRegister(uint8_t reg, uint8_t value) {
    if (!_i2c) return false;
    uint8_t buf[2] = {reg, value};
    int ret = i2c_write_blocking(_i2c, MPU6050_ADDR, buf, 2, false);
    return (ret == 2);
}

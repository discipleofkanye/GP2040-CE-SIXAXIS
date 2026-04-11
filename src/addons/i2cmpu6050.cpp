#include "addons/i2cmpu6050.h"
#include "MPU6050.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "storagemanager.h"

static MPU6050 mpu;
static bool initialized = false;

void I2CMPU6050Input::setup() {
    uint8_t sda = 4;
    uint8_t scl = 5;

    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    if (mpu.begin(i2c0, sda, scl)) {
        initialized = true;
    }
}

void I2CMPU6050Input::process() {
    if (!initialized) return;

    int16_t ax, ay, az, gx, gy, gz;
    if (mpu.readRawAccel(ax, ay, az) && mpu.readRawGyro(gx, gy, gz)) {
        Gamepad* gamepad = Storage::getInstance().GetGamepad();
        gamepad->state.accelX = ax;
        gamepad->state.accelY = ay;
        gamepad->state.accelZ = az;
        gamepad->state.gyroZ = gz;
    }
}

void I2CMPU6050Input::postprocess(bool) {}
void I2CMPU6050Input::reinit() {}

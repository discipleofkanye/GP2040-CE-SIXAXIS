#include "addons/i2cmpu6050.h"
#include "MPU6050.h"
#include "peripheral_i2c.h"

static MPU6050 mpu;
static bool initialized = false;

void I2CMPU6050Input::setup() {
    const AddonOptions& addonOptions = Storage::getInstance().getAddonOptions();
    const MPU6050Options& options = addonOptions.mpu6050Options;
    
    if (!options.enabled) return;
    
    // Get I2C pins from board config or defaults
    uint8_t sda = options.i2cSdaPin;
    uint8_t scl = options.i2cSclPin;
    if (sda == 0 && scl == 0) {
        // Use default I2C0 pins (often GPIO 4 and 5)
        sda = 4;
        scl = 5;
    }
    
    // Initialize I2C at 400kHz
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
        // Convert raw values to something usable (scale later in PS3Driver)
        gamepad->state.accelX = ax;
        gamepad->state.accelY = ay;
        gamepad->state.accelZ = az;
        gamepad->state.gyroZ = gz;  // Only Z gyro used by Sixaxis
    }
}

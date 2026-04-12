#include "mcp3208.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "storagemanager.h"

bool MCP3208Input::available() {
    return initialized;
}

void MCP3208Input::spiInit() {
    spi_init(MCP3208_SPI_BLOCK, 1000 * 1000);
    spi_set_format(MCP3208_SPI_BLOCK, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    gpio_set_function(MCP3208_CS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(MCP3208_CS_PIN, GPIO_OUT);
    gpio_put(MCP3208_CS_PIN, 1);
    
    gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI (DIN)
    gpio_set_function(16, GPIO_FUNC_SPI);  // MISO (DOUT)
    gpio_set_function(18, GPIO_FUNC_SPI);  // SCK (CLK)
}

uint8_t MCP3208Input::readChannel(uint8_t channel) {
    if (channel > 7) return 0;
    
    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];
    
    txBuffer[0] = 0x06 | ((channel >> 2) & 0x01);
    txBuffer[1] = ((channel & 0x03) << 6);
    txBuffer[2] = 0x00;
    
    gpio_put(MCP3208_CS_PIN, 0);
    spi_write_read_blocking(MCP3208_SPI_BLOCK, txBuffer, rxBuffer, 3);
    gpio_put(MCP3208_CS_PIN, 1);
    
    uint16_t result = ((rxBuffer[1] & 0x0F) << 8) | rxBuffer[2];
    return (result >> 4);  // scale 12-bit to 8-bit (0-255)
}

void MCP3208Input::setup() {
    spiInit();
    for (int i = 0; i < 4; i++) {
        stickValues[i] = readChannel(i);
    }
    initialized = true;
}

void MCP3208Input::process() {
    if (!initialized) return;
    
    Gamepad* gamepad = Storage::getInstance().GetGamepad();
    
    stickValues[0] = readChannel(0);  // Left X
    stickValues[1] = readChannel(1);  // Left Y
    stickValues[2] = readChannel(2);  // Right X
    stickValues[3] = readChannel(3);  // Right Y
    
    gamepad->state.lx = stickValues[0];
    gamepad->state.ly = stickValues[1];
    gamepad->state.rx = stickValues[2];
    gamepad->state.ry = stickValues[3];
}

#include "mcp3208.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"
#include "storagemanager.h"

bool MCP3208Input::available() {
    return initialized;
}

void MCP3208Input::spiInit() {
    // Initialize SPI at 1MHz
    spi_init(MCP3208_SPI_BLOCK, 1000 * 1000);
    spi_set_format(MCP3208_SPI_BLOCK, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    // Set up the CS pin
    gpio_set_function(MCP3208_CS_PIN, GPIO_FUNC_SIO);
    gpio_set_dir(MCP3208_CS_PIN, GPIO_OUT);
    gpio_put(MCP3208_CS_PIN, 1);
    
    // Set up the SPI pins
    gpio_set_function(19, GPIO_FUNC_SPI);  // MOSI (DIN)
    gpio_set_function(16, GPIO_FUNC_SPI);  // MISO (DOUT)
    gpio_set_function(18, GPIO_FUNC_SPI);  // SCK (CLK)
}

uint8_t MCP3208Input::readChannel(uint8_t channel) {
    if (channel > 7) return 0;
    
    // MCP3208 communication protocol:
    // First byte: start bit (1) + single-ended mode (1) + channel selection (3 bits)
    // Second byte: don't care
    // The chip returns 12 bits of data over the next 2 bytes
    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];
    
    // Build the command: [start, single-ended, channel (MSB), channel (LSB)]
    // For single-ended mode, the command is:
    // Byte 0: 0x06 (start + single-ended) OR (channel >> 2)
    // Byte 1: (channel & 0x03) << 6
    // Byte 2: 0x00 (don't care)
    txBuffer[0] = 0x06 | ((channel >> 2) & 0x01);
    txBuffer[1] = ((channel & 0x03) << 6);
    txBuffer[2] = 0x00;
    
    // Perform the SPI transaction
    gpio_put(MCP3208_CS_PIN, 0);
    spi_write_read_blocking(MCP3208_SPI_BLOCK, txBuffer, rxBuffer, 3);
    gpio_put(MCP3208_CS_PIN, 1);
    
    // Extract the 12-bit result from the last two bytes
    // rxBuffer[1] contains the high 8 bits, rxBuffer[2] contains the low 4 bits
    uint16_t result = ((rxBuffer[1] & 0x0F) << 8) | rxBuffer[2];
    
    // Scale from 12-bit (0-4095) to 8-bit (0-255) for the gamepad state
    return (result >> 4);
}

void MCP3208Input::setup() {
    spiInit();
    
    // Read all four channels once to ensure the SPI bus is working
    for (int i = 0; i < 4; i++) {
        stickValues[i] = readChannel(i);
    }
    initialized = true;
}

void MCP3208Input::process() {
    if (!initialized) return;
    
    Gamepad* gamepad = Storage::getInstance().GetGamepad();
    
    // Read the four analog axes
    stickValues[0] = readChannel(0);  // Left Stick X
    stickValues[1] = readChannel(1);  // Left Stick Y
    stickValues[2] = readChannel(2);  // Right Stick X
    stickValues[3] = readChannel(3);  // Right Stick Y
    
    // Assign the values to the gamepad state
    gamepad->state.lx = stickValues[0];
    gamepad->state.ly = stickValues[1];
    gamepad->state.rx = stickValues[2];
    gamepad->state.ry = stickValues[3];
    
    // Optional: Invert axes if needed
    // gamepad->state.ly = 255 - stickValues[1];  // Invert Y
}

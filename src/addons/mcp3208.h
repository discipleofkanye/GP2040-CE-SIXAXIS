#pragma once

#include "gpaddon.h"
#include "Gamepad.h"

#ifndef MCP3208_ENABLED
#define MCP3208_ENABLED 0
#endif

#ifndef MCP3208_CS_PIN
#define MCP3208_CS_PIN 17
#endif

#ifndef MCP3208_SPI_BLOCK
#define MCP3208_SPI_BLOCK spi0
#endif

class MCP3208Input : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void process();
    virtual void preprocess() {}
    virtual void postprocess(bool) {}
    virtual void reinit() {}
    virtual std::string name() { return "MCP3208"; }

private:
    uint8_t readChannel(uint8_t channel);
    void spiInit();

    bool initialized = false;
    uint16_t stickValues[4];  // LX, LY, RX, RY
};

#pragma once
#include <stdint.h>

// DS3502 I2C digital potentiometer (7-bit, 0–127).
// I2C address: 0x28 | (AD1 << 1) | AD0  → 0x28–0x2B.

class DigitalPot {
public:
    explicit DigitalPot(uint8_t i2c_addr = 0x28);

    // Initialise I2C and write the starting wiper position.
    void setup(uint8_t initial_value = 0);

    // Set wiper position. Percentage 0–100; values above 100 are clamped to 100.
    void write(uint8_t percent);

private:
    uint8_t m_addr;
};

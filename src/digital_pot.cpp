#include "digital_pot.h"
#include <Wire.h>

static constexpr uint8_t DS3502_REG_WIPER = 0x00;
static constexpr uint8_t DS3502_MAX_VALUE  = 127;

DigitalPot::DigitalPot(uint8_t i2c_addr) : m_addr(i2c_addr) {}

void DigitalPot::setup(uint8_t initial_value) {
    Wire.begin();
    write(initial_value);
}

void DigitalPot::write(uint8_t percent) {
    if (percent > 100) percent = 100;
    uint8_t value = (uint8_t)((uint16_t)percent * DS3502_MAX_VALUE / 100);
    Wire.beginTransmission(m_addr);
    Wire.write(DS3502_REG_WIPER);
    Wire.write(value);
    Wire.endTransmission();
}

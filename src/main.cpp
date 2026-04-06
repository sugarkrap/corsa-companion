#include <Arduino.h>
#include "config.h"
#include "adc_reader.h"
#include "button.h"
#include "oled_display.h"
#include "serial_cmd.h"
#include "tid_display.h"
#include "digital_pot.h"

static TIDDisplay    s_tid;
static DigitalPot    s_pot;

static unsigned long s_pot_active_until_ms = 0;
static unsigned long s_last_sample_ms      = 0;
static ButtonValue   s_last_button         = ButtonValue::O;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(SERIAL_INIT_DELAY_MS);

    // oled_init();
    adc_init();
    s_pot.setup(0);

    Serial.println("# Digital pot test — send A/S/D/F to trigger 11/25/60/80% for 400 ms");
}

void loop() {
    // ── Serial key handling ───────────────────────────────────────────────
    if (Serial.available()) {
        char ch = (char)Serial.read();
        uint8_t pct = 100;
        switch (ch) {
            case 'A': case 'a': pct = 0; break;
            case 'S': case 's': pct = 12; break;
            case 'D': case 'd': pct = 24; break;
            case 'F': case 'f': pct = 36; break;
            default: break;
        }
        if (pct < 100) {
            s_pot.write(pct);
            s_pot_active_until_ms = millis() + 150;
            Serial.print("# pot -> ");
            Serial.print(pct);
            Serial.println("%");
        }
    }

    // ── Return to idle when pulse expires ─────────────────────────────────
    if (s_pot_active_until_ms != 0 && millis() >= s_pot_active_until_ms) {
        s_pot_active_until_ms = 0;
        s_pot.write(100);
        Serial.println("# pot -> 100% (idle)");
    }

    // ── ADC sampling + button detection ───────────────────────────────────
    unsigned long now = millis();
    if (now - s_last_sample_ms >= static_cast<unsigned long>(SAMPLE_INTERVAL_MS)) {
        s_last_sample_ms = now;

        float voltage = adc_read_voltage_avg();
        const Button* btn = button_decode(voltage);

        if (btn) {
            if (btn->value != s_last_button) {
                Serial.print("# Button: ");
                Serial.print(btn->label);
                Serial.print("  voltage: ");
                Serial.print(voltage, 3);
                Serial.println(" V");
            }
        } else {
            Serial.print("# Odd reading: ");
            Serial.print(voltage, 3);
            Serial.println(" V");
        }
        s_last_button = btn ? btn->value : ButtonValue::O;
    }
}

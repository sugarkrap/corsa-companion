#include <Arduino.h>
#include "config.h"
#include "adc_reader.h"
#include "serial_cmd.h"
#include "sleep_mode.h"

static unsigned long s_last_sample_ms = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(SERIAL_INIT_DELAY_MS);

    adc_init();
    serial_cmd_init();
    sleep_mode_init();

    Serial.println("# SWC voltage calibration");
    Serial.println("# Serial commands: ON / OFF");
    Serial.println("# Button A: toggle sleep mode");
    Serial.println("# Plotter columns: Voltage_V");
}

void loop() {
    // ── Button A — sleep toggle ────────────────────────────────────────────
    if (sleep_mode_poll()) {
        Serial.println(sleep_mode_is_sleeping() ? "# SLEEP" : "# AWAKE");
    }

    if (sleep_mode_is_sleeping()) {
        return;
    }

    // ── Serial command handling ────────────────────────────────────────────
    SerialCmd cmd = serial_cmd_poll();
    if (cmd == CMD_ON) {
        Serial.println("# Illumination ON");
    } else if (cmd == CMD_OFF) {
        Serial.println("# Illumination OFF");
    }

    // ── ADC sampling at fixed interval ────────────────────────────────────
    unsigned long now = millis();
    if (now - s_last_sample_ms >= static_cast<unsigned long>(SAMPLE_INTERVAL_MS)) {
        s_last_sample_ms = now;

        Serial.print("Voltage_V:");
        Serial.println(adc_read_voltage(), 3);
    }
}

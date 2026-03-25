#include <Arduino.h>
#include "config.h"
#include "adc_reader.h"
#include "serial_cmd.h"
#include "tid_writer.h"

static unsigned long s_last_sample_ms = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(SERIAL_INIT_DELAY_MS);

    adc_init();
    serial_cmd_init();
    tid_writer_init(true);

    Serial.println("# SWC voltage calibration");
    Serial.println("# Serial commands: ON / OFF");
    Serial.println("# Plotter columns: Voltage_V");
}

void loop() {
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

    // ── TID display ───────────────────────────────────────────────────────
    sendTID(0x00, 0x00, 0x00, "HELLO     ");
}

#include <Arduino.h>
#include "config.h"
#include "adc_reader.h"
#include "button.h"
#include "oled_display.h"
#include "serial_cmd.h"
#include "tid_writer.h"

static unsigned long s_last_sample_ms = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(SERIAL_INIT_DELAY_MS);

    oled_init();
    adc_init();
    serial_cmd_init();
    tid_writer_init(true);

    Serial.println("# SWC voltage calibration");
    Serial.println("# Serial commands: ON / OFF");
<<<<<<< HEAD
    Serial.println("# Plotter columns: Voltage_V");
=======
    Serial.println("# Button A: toggle sleep mode");
>>>>>>> f9cd1c4 (oled and buttons)
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

        float voltage = adc_read_voltage_avg();
        oled_show_button(button_decode(voltage), voltage);
    }

    // ── TID display ───────────────────────────────────────────────────────
    sendTID(0x00, 0x00, 0x00, "HELLO     ");
}

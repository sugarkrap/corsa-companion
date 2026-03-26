#include <Arduino.h>
#include "config.h"
#include "adc_reader.h"
#include "button.h"
#include "oled_display.h"
#include "serial_cmd.h"
#include "tid_display.h"

static unsigned long s_last_sample_ms = 0;
static TIDDisplay    s_tid;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(SERIAL_INIT_DELAY_MS);

    oled_init();
    adc_init();
    serial_cmd_init();
    s_tid.setup();
    s_tid.display_text("Hello World!");

    Serial.println("# SWC voltage calibration");
    Serial.println("# Serial commands: ON / OFF / SYM <1-11> <0|1>");
    Serial.println("# Plotter columns: Voltage_V");
}

void loop() {
    // ── Serial command handling ────────────────────────────────────────────
    SerialCmd cmd = serial_cmd_poll();
    if (cmd == CMD_ON) {
        Serial.println("# Illumination ON");
    } else if (cmd == CMD_OFF) {
        Serial.println("# Illumination OFF");
    } else if (cmd == CMD_SYM) {
        s_tid.set_symbol(serial_cmd_sym_number(), serial_cmd_sym_state());
    }

    s_tid.update();

    // ── ADC sampling ──────────────────────────────────────────────────────
    unsigned long now = millis();
    if (now - s_last_sample_ms >= static_cast<unsigned long>(SAMPLE_INTERVAL_MS)) {
        s_last_sample_ms = now;

        float voltage = adc_read_voltage_avg();
        oled_show_button(button_decode(voltage), voltage);
    }
}

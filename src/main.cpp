#include <Arduino.h>
#include "tid_display.h"

static TIDDisplay s_tid;
static char s_serial_buf[TID_TEXT_BUF];
static uint8_t s_serial_len = 0;

void setup() {
    Serial.begin(9600);

    s_tid.setup();
    s_tid.display_text("Ready");
}

void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (s_serial_len > 0) {
                s_serial_buf[s_serial_len] = '\0';
                s_tid.display_text(s_serial_buf);
                s_serial_len = 0;
            }
        } else if (s_serial_len < TID_TEXT_BUF - 1) {
            s_serial_buf[s_serial_len++] = c;
        }
    }

    s_tid.update();
    delay(10);
}

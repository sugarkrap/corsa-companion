#include <Arduino.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "config.h"
#include "tid_display.h"

// ── Idle phrases (PROGMEM to spare RAM) ───────────────────────────────────────
static const char s_p0[] PROGMEM = "Welcome!";
static const char s_p1[] PROGMEM = "Opel Corsa C";
static const char s_p2[] PROGMEM = "1.2L Twinport";
static const char s_p3[] PROGMEM = "Have a safe ride";
static const char s_p4[] PROGMEM = "Off to a joyride?";
static const char s_p5[] PROGMEM = "Wir leben Autos";
static const char* const s_idle_phrases[] PROGMEM = {
    s_p0, s_p1, s_p2, s_p3, s_p4, s_p5,
};
static constexpr uint8_t IDLE_PHRASE_COUNT = sizeof(s_idle_phrases) / sizeof(s_idle_phrases[0]);

static constexpr unsigned long IDLE_PHRASE_INTERVAL_MS = 8000;
static constexpr unsigned long IDLE_RETURN_TIMEOUT_MS  = 30000;

// ── State ─────────────────────────────────────────────────────────────────────
static TIDDisplay s_tid;
static char s_serial_buf[TID_TEXT_BUF];
static uint8_t s_serial_len = 0;

static bool          s_idle_mode      = true;
static uint8_t       s_idle_idx       = 0;
static unsigned long s_next_phrase_ms = 0;
static unsigned long s_last_rx_ms     = 0;

static void show_next_idle_phrase() {
    char buf[TID_TEXT_BUF];
    strcpy_P(buf, (PGM_P)pgm_read_word(&s_idle_phrases[s_idle_idx]));
    s_tid.display_text(buf);
    s_idle_idx = (s_idle_idx + 1) % IDLE_PHRASE_COUNT;
    s_next_phrase_ms = millis() + IDLE_PHRASE_INTERVAL_MS;
}

void setup() {
    Serial.begin(9600);
    s_tid.setup();
    show_next_idle_phrase();
}

void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            if (s_serial_len > 0) {
                s_serial_buf[s_serial_len] = '\0';
                if (strcmp(s_serial_buf, "version") == 0) {
                    Serial.println(FIRMWARE_VERSION);
                } else {
                    s_tid.display_text(s_serial_buf);
                    s_idle_mode  = false;
                    s_last_rx_ms = millis();
                    Serial.println("ok");
                }
                s_serial_len = 0;
            }
        } else if (s_serial_len < TID_TEXT_BUF - 1) {
            s_serial_buf[s_serial_len++] = c;
        }
    }

    unsigned long now = millis();

    if (!s_idle_mode && (now - s_last_rx_ms >= IDLE_RETURN_TIMEOUT_MS)) {
        s_idle_mode      = true;
        s_idle_idx       = 0;
        s_next_phrase_ms = 0;
    }

    if (s_idle_mode && now >= s_next_phrase_ms) {
        show_next_idle_phrase();
    }

    s_tid.update();
    delay(10);
}

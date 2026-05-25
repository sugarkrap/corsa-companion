#include "tid_display.h"
#include "tid_writer.h"
#include <Arduino.h>
#include <string.h>

TIDDisplay::TIDDisplay()
    : _text_len(0), _scroll_pos(0), _last_scroll_ms(0), _pause_until_ms(0) {
    memset(_text, 0, sizeof(_text));
    memset(_frame, ' ', 10);
    _frame[10] = '\0';
}

void TIDDisplay::setup() {
    tid_writer_init(true);
}

void TIDDisplay::display_text(const char* text) {
    strncpy(_text, text, sizeof(_text) - 1);
    _text[sizeof(_text) - 1] = '\0';
    _text_len       = (uint8_t)strlen(_text);
    _scroll_pos     = 0;
    _last_scroll_ms = millis();
    _pause_until_ms = millis() + SCROLL_END_PAUSE;
    _build_frame();
    _send();
}

void TIDDisplay::update() {
    // The TID holds its display after a single write — no continuous refresh needed.
    // Only send when the scroll position advances.
    if (_text_len <= 10) return;

    unsigned long now = millis();
    if (now < _pause_until_ms) return;

    if (now - _last_scroll_ms >= SCROLL_INTERVAL) {
        _last_scroll_ms = now;
        _scroll_pos++;
        if (_scroll_pos > _text_len - 10) {
            _scroll_pos     = 0;
            _pause_until_ms = now + SCROLL_END_PAUSE;
        } else if (_scroll_pos == _text_len - 10) {
            _pause_until_ms = now + SCROLL_END_PAUSE;
        }
        _build_frame();
        _send();
    }
}

void TIDDisplay::set_symbol(uint8_t symbo, bool on) {
    tid_set_symbol(symbo, on);
}

void TIDDisplay::_build_frame() {
    for (uint8_t i = 0; i < 10; i++) {
        uint8_t idx = _scroll_pos + i;
        _frame[i] = (idx < _text_len) ? _text[idx] : ' ';
    }
    _frame[10] = '\0';
}

void TIDDisplay::_send() {
    static unsigned long s_last_log_ms = 0;
    unsigned long now = millis();
    if (now - s_last_log_ms >= 5000) {
        s_last_log_ms = now;
        Serial.print(F("[TID] frame: \""));
        Serial.print(_frame);
        Serial.println(F("\""));
    }
    sendTID(_frame);
}

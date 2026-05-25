#pragma once
#include <stdint.h>

#define SCROLL_INTERVAL   500   // ms between scroll steps
#define SCROLL_END_PAUSE  3000  // ms to hold at start and end of scroll
#define TID_TEXT_BUF    64   // max text length accepted by display_text()

class TIDDisplay {
public:
    TIDDisplay();
    void setup();
    void update();
    void display_text(const char* text);
    void set_symbol(uint8_t symbo, bool on);

private:
    char          _text[TID_TEXT_BUF];
    uint8_t       _text_len;
    uint8_t       _scroll_pos;
    unsigned long _last_scroll_ms;
    unsigned long _pause_until_ms;
    char          _frame[11];  // 10 visible chars + null

    void _build_frame();
    void _send();
};

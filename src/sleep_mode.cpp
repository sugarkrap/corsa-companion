#include "sleep_mode.h"
#include "config.h"

static constexpr int DEBOUNCE_COUNT = 5;

static bool s_sleeping     = false;
static bool s_last_pressed = false;
static int  s_debounce     = 0;

void sleep_mode_init() {
    pinMode(PIN_BTN_SLEEP, INPUT_PULLUP);
}

bool sleep_mode_poll() {
    bool pressed = (digitalRead(PIN_BTN_SLEEP) == LOW);

    if (pressed) {
        if (s_debounce < DEBOUNCE_COUNT) s_debounce++;
    } else {
        s_debounce = 0;
    }

    bool debounced = (s_debounce >= DEBOUNCE_COUNT);

    if (debounced && !s_last_pressed) {
        s_last_pressed = true;
        s_sleeping = !s_sleeping;
        return true;
    }
    if (!debounced) s_last_pressed = false;
    return false;
}

bool sleep_mode_is_sleeping() {
    return s_sleeping;
}

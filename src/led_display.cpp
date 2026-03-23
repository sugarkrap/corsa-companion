#include "led_display.h"
#include "config.h"

void led_display_init() {
    pinMode(PIN_LED_STATUS, OUTPUT);
    digitalWrite(PIN_LED_STATUS, LOW);
}

void led_display_illum_on() {
    digitalWrite(PIN_LED_STATUS, HIGH);
}

void led_display_illum_off() {
    digitalWrite(PIN_LED_STATUS, LOW);
}

void led_display_sleep_tick() {
    static unsigned long last_toggle = 0;
    static bool          led_on      = false;

    unsigned long now = millis();
    if (now - last_toggle < 500) return;
    last_toggle = now;
    led_on = !led_on;
    digitalWrite(PIN_LED_STATUS, led_on ? HIGH : LOW);
}

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

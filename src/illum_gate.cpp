#include "illum_gate.h"
#include "config.h"

static bool s_on = false;

void illum_init() {
    pinMode(PIN_ILLUM, OUTPUT);
    digitalWrite(PIN_ILLUM, LOW);
    s_on = false;
}

void illum_on() {
    digitalWrite(PIN_ILLUM, HIGH);
    s_on = true;
}

void illum_off() {
    digitalWrite(PIN_ILLUM, LOW);
    s_on = false;
}

bool illum_is_on() {
    return s_on;
}
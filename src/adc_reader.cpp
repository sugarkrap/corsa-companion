#include "adc_reader.h"
#include "config.h"

void adc_init() {
    // DEFAULT = 5 V reference. Required when using the internal pullup as bias
    // (no external divider) — button voltages reach up to 5 V so the 1.1 V
    // reference would clip Prev, Source, and None indistinguishably at 1023.
    analogReference(DEFAULT);
    pinMode(PIN_ADC, INPUT_PULLUP);
}

int adc_read_raw() {
    return analogRead(PIN_ADC);
}

float adc_read_voltage() {
    return adc_read_raw() * (ADC_REF_VOLTAGE / ADC_MAX_RAW);
}
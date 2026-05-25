#include <Arduino.h>
#include "adc_reader.h"
#include "config.h"

void adc_init() {
    analogReference(INTERNAL); // 1.1 V bandgap; first conversion must be discarded.
    analogRead(PIN_ADC);       // dummy read to let the reference settle.
}

// Collect samples, insertion-sort in place, return the middle value.
// Rejects spike outliers completely — a single bad sample cannot shift the result.
static int adc_median_raw() {
    int buf[ADC_AVERAGE_SAMPLES];
    for (int i = 0; i < ADC_AVERAGE_SAMPLES; ++i) {
        analogRead(PIN_ADC); // dummy — settle mux
        buf[i] = analogRead(PIN_ADC);
    }
    // Insertion sort (N is small, no heap needed)
    for (int i = 1; i < ADC_AVERAGE_SAMPLES; ++i) {
        int key = buf[i];
        int j   = i - 1;
        while (j >= 0 && buf[j] > key) { buf[j + 1] = buf[j]; --j; }
        buf[j + 1] = key;
    }
    return buf[ADC_AVERAGE_SAMPLES / 2];
}

float adc_read_voltage_avg() {
    return (static_cast<float>(adc_median_raw()) / ADC_MAX_RAW) * ADC_REF_VOLTAGE;
}

int adc_read_raw_avg() {
    return adc_median_raw();
}

int adc_read_raw() {
    analogRead(PIN_ADC); // dummy — settle reference
    return analogRead(PIN_ADC);
}

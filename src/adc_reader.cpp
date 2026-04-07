#include <Arduino.h>
#include "adc_reader.h"
#include "config.h"

void adc_init() {
    analogReference(DEFAULT); // 1.1 V bandgap; first conversion must be discarded.
    analogRead(PIN_ADC);       // dummy read to let the reference settle.
}

float adc_read_voltage_avg() {
    long sum = 0;
    for (int i = 0; i < ADC_AVERAGE_SAMPLES; ++i) {
        analogRead(PIN_ADC); // dummy — settle after any previous channel switch
        sum += analogRead(PIN_ADC);
    }
    float avg = static_cast<float>(sum) / ADC_AVERAGE_SAMPLES;
    return (avg / ADC_MAX_RAW) * ADC_REF_VOLTAGE;
}

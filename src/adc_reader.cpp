#include "adc_reader.h"
#include "config.h"


void adc_init() {
    // DEFAULT = 5 V reference. Required when using the internal pullup as bias
    // (no external divider) — button voltages reach up to 5 V so the 1.1 V
    // reference would clip Prev, Source, and None indistinguishably at 1023.
    analogReference(DEFAULT);
    pinMode(PIN_ADC,     INPUT_PULLUP); // A0: SWC+
    pinMode(PIN_ADC_REF, INPUT_PULLUP); // A1: SWC- (floating; reads near Vcc as ratiometric ref)
}

int adc_read_raw() {
    return analogRead(PIN_ADC);
}

int adc_read_ref_raw() {
    return analogRead(PIN_ADC_REF);
}

float adc_read_voltage() {
    float sig = static_cast<float>(adc_read_raw());
    float ref = static_cast<float>(adc_read_ref_raw());
    return (sig / ref) * ADC_REF_VOLTAGE;
}

float adc_read_voltage_avg() {
    long sum_sig = 0;
    long sum_ref = 0;
    for (int i = 0; i < ADC_AVERAGE_SAMPLES; ++i) {
        sum_sig += adc_read_raw();
        sum_ref += adc_read_ref_raw();
    }
    float avg_sig = static_cast<float>(sum_sig) / ADC_AVERAGE_SAMPLES;
    float avg_ref = static_cast<float>(sum_ref) / ADC_AVERAGE_SAMPLES;
    return (avg_sig / avg_ref) * ADC_REF_VOLTAGE;
}
#pragma once

void  adc_init();
float adc_read_voltage_avg(); // averaged VADC in volts (A1)
int   adc_read_raw_avg();     // averaged raw 10-bit ADC count (A1)
int   adc_read_raw();         // single raw 10-bit ADC read (A1)

#pragma once

// Reads the SWC signal on P1 (A0) and converts to voltage.
// Illumination gating is the caller's responsibility.

void  adc_init();
int   adc_read_raw();
float adc_read_voltage();
float adc_read_voltage_avg();
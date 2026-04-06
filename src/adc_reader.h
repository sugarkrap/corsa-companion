#pragma once

// Reads the SWC signal on P1 (A0) and converts to voltage.
// Illumination gating is the caller's responsibility.

void  adc_init();
int   adc_read_raw();        // raw count from A0 (SWC+)
int   adc_read_ref_raw();    // raw count from A1 (SWC- pullup reference)
float adc_read_voltage();    // ratiometric corrected voltage
float adc_read_voltage_avg(); // averaged ratiometric corrected voltage
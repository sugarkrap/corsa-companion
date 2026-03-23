#pragma once

// Button A toggles sleep mode.
// In sleep mode the main loop skips ADC sampling and serial output.
// The LED matrix blinks slowly to signal standby.
//
// Button A (pin 5) is shared with LED col 1 — the read momentarily
// reconfigures the pin, so call this *between* LED matrix updates.

void sleep_mode_init();

// Call every loop iteration. Returns true on the cycle the mode toggles.
bool sleep_mode_poll();

bool sleep_mode_is_sleeping();
#pragma once

// Status LED (D13 built-in) — visual feedback during bench work.
// No LED matrix on the Nano; a single LED is enough for state indication.

void led_display_init();
void led_display_illum_on();    // LED solid on  → illumination gate active
void led_display_illum_off();   // LED off       → illumination gate inactive

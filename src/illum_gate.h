#pragma once

// Controls the illumination gate pin (P2 / A1).
// On the bench this pin is driven as OUTPUT to simulate the car's LED+ rail.
// Call init() once in setup(), then on()/off() from serial commands.

void illum_init();
void illum_on();
void illum_off();
bool illum_is_on();
#pragma once
#include "button.h"

void oled_init();
void oled_hello();
void oled_show_voltage(float voltage);
void oled_show_button(const Button* btn, float voltage);

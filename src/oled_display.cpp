#include "oled_display.h"
#include "button.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

static Adafruit_SSD1306 s_oled(128, 64, &Wire, -1);

void oled_init() {
    if (!s_oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        return; // display not found — continue without it
    }
    oled_hello();
}

void oled_hello() {
    s_oled.clearDisplay();
    s_oled.setTextSize(2);
    s_oled.setTextColor(SSD1306_WHITE);
    s_oled.setCursor(0, 24);
    s_oled.println("Hello World");
    s_oled.display();
}

void oled_show_voltage(float voltage) {
    s_oled.clearDisplay();
    s_oled.setTextColor(SSD1306_WHITE);

    s_oled.setTextSize(1);
    s_oled.setCursor(0, 0);
    s_oled.println("SWC Voltage");

    s_oled.setTextSize(2);
    s_oled.setCursor(0, 20);
    s_oled.print(voltage, 3);
    s_oled.println(" V");

    s_oled.display();
}

void oled_show_button(const Button* btn, float voltage) {
    s_oled.clearDisplay();
    s_oled.setTextColor(SSD1306_WHITE);

    s_oled.setTextSize(1);
    s_oled.setCursor(0, 0);
    s_oled.println("Button");

    s_oled.setTextSize(4);
    s_oled.setCursor(52, 16);
    s_oled.println(btn ? btn->label : '?');

    s_oled.setTextSize(1);
    s_oled.setCursor(32, 52);
    s_oled.print(voltage, 3);
    s_oled.println(" V");

    s_oled.display();
}

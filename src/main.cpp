#include <Arduino.h>
#include <nrf51.h>

// LED matrix pin definitions for BBC micro:bit
const int LED_ROWS[] = {21, 22, 15, 24, 19};
const int LED_COLS[] = {28, 11, 31, 5, 30};

void all_leds_off(void) {
    for (int i = 0; i < 5; i++) {
        pinMode(LED_ROWS[i], OUTPUT);
        digitalWrite(LED_ROWS[i], LOW);
        pinMode(LED_COLS[i], OUTPUT);
        digitalWrite(LED_COLS[i], HIGH);
    }
}

void all_leds_on(void) {
    for (int i = 0; i < 5; i++) {
        pinMode(LED_ROWS[i], OUTPUT);
        digitalWrite(LED_ROWS[i], HIGH);
        pinMode(LED_COLS[i], OUTPUT);
        digitalWrite(LED_COLS[i], LOW);
    }
}

void enter_deep_sleep(void) {
    all_leds_off();
    for (int pin = 0; pin < 32; pin++) {
        pinMode(pin, INPUT_PULLDOWN);
    }
    NRF_POWER->SYSTEMOFF = 1;
    __DSB();
    __WFI();
}

void setup() {
    // Turn on LED first to show we're alive
    all_leds_on();
    
    // Initialize serial communication
    Serial.begin(9600);
    
    // Give USB CDC time to initialize (micro:bit needs this)
    delay(2000);
    
    // Say Hello World over serial!
    Serial.println("Hello World from micro:bit!");
    Serial.println("======================");
    Serial.flush();  // Ensure message is sent before continuing
    
    // Wait a bit so you can see the message
    delay(3000);

    // Turn off all LEDs
    all_leds_off();

    // Wait 1 second
    delay(1000);

    // Turn all LEDs back on
    all_leds_on();

    // Wait 1 second with LEDs on
    delay(1000);

    // Enter deep sleep (LEDs off)
    enter_deep_sleep();
}

void loop() {
    // Never reached
}

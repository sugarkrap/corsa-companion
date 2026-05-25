#include <Arduino.h>
#include "tid_display.h"

static TIDDisplay s_tid;

void setup() {
    Serial.begin(9600);
    s_tid.setup();
    s_tid.display_text("Waiting");
    Serial.println(F("[diag] TID diagnostic firmware running"));
}

void loop() {
    s_tid.update();
    delay(10);
}

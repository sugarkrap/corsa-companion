#pragma once

enum class ButtonValue { Left, Right, Up, Plus, Minus, O };

struct VoltageRange {
    float min;
    float max;
};

struct Button {
    VoltageRange range;
    ButtonValue  value;
    char         label;
};

// Returns a pointer to the matched Button, or nullptr if no button matches.
const Button* button_decode(float voltage);

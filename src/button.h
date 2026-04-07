#pragma once

enum class ButtonValue { Left, Right, Up, Plus, Minus, O };

struct PercentRange {
    float min;
    float max;
};

struct ButtonPct {
    PercentRange range;
    float        vref;   // idle VADC at calibration (V) — informational
    ButtonValue  value;
    char         label;
};

// Low-level: match a pct value against the button table.
const ButtonPct* button_decode_pct(float pct);

// State machine — call once per sample with the raw VADC.
// Returns the matched button on the IDLE→PRESSED transition only.
// Returns nullptr while idle or while waiting for release.
const ButtonPct* button_update(float vadc);

// Current idle reference voltage, for debug output.
float button_idle_vadc();

// Returns true exactly once after a PRESSED→IDLE transition (button released).
bool button_just_went_idle();

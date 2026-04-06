#include "button.h"


// My buttons, right doesn't work so it isn't there
static const Button BUTTONS[] = {
    {{1.747f, 1.849f}, ButtonValue::Minus, '-'},
    {{1.850f, 1.859f}, ButtonValue::Plus,  '+'},
    {{1.860f, 1.869f}, ButtonValue::O,     'O'},
    {{1.890f, 1.910f}, ButtonValue::Left,  '<'},
    {{1.870f, 1.880f}, ButtonValue::Right, '>'},
    {{1.940f, 1.950f}, ButtonValue::Up, '^'}
};

static constexpr int BUTTON_COUNT = sizeof(BUTTONS) / sizeof(BUTTONS[0]);

const Button* button_decode(float voltage) {
    for (int i = 0; i < BUTTON_COUNT; ++i) {
        if (voltage >= BUTTONS[i].range.min && voltage <= BUTTONS[i].range.max) {
            return &BUTTONS[i];
        }
    }
    return nullptr;
}

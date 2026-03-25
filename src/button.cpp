#include "button.h"


// My buttons, right doesn't work so it isn't there
static const Button BUTTONS[] = {
    {{1.718f, 1.728f}, ButtonValue::Minus, '-'},
    {{1.728f, 1.738f}, ButtonValue::Plus,  '+'},
    {{1.738f, 1.748f}, ButtonValue::O,     'O'},
    {{1.775f, 1.793f}, ButtonValue::Left,  '<'},
    {{1.808f, 1.878f}, ButtonValue::Up, '^'}
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

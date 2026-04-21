#include "button.h"

static inline float absf(float x) { return x < 0.0f ? -x : x; }

// Deviation from idle that triggers a press (% of idle voltage).
static constexpr float PRESS_THRESHOLD_PCT   = 3.0f;
// Deviation below which we consider the button released.
// Must be < PRESS_THRESHOLD to avoid chattering at the boundary.
static constexpr float RELEASE_THRESHOLD_PCT = 2.0f;
// Downward tracking is fast — recovers quickly after a press releases.
static constexpr float IDLE_ALPHA_DOWN = 0.60f;
// Upward tracking is slow — follows genuine baseline drift without
// chasing button presses upward.
static constexpr float IDLE_ALPHA_UP   = 0.05f;
// Consecutive samples that must agree before a press is registered.
static constexpr int   CONFIRM_SAMPLES = 3;

// ── Button table ─────────────────────────────────────────────────────────────
// range: pct = (vadc / idle_vadc) * 100 at press time.
// vref:  idle_vadc observed during calibration (informational).
// DUMMY RANGES — widen after observing real pct values from debug output.
static const ButtonPct BUTTONS_PCT[] = {
    {{107.43f, 108.43f}, 0.791f, ButtonValue::Minus, '-'},
    {{110.54f, 111.54f}, 0.818f, ButtonValue::Plus,  '+'},
    {{81.80f, 82.50f}, 2.387f, ButtonValue::O,     'O'},
    {{83.00f, 84.50f}, 2.393f, ButtonValue::Right, '>'},
    {{89.30f, 90.20f}, 2.366f, ButtonValue::Up,    '^'},
    {{  0.0f,   0.0f}, 0.0f,   ButtonValue::Left,  '<'}, // not yet calibrated
};

static constexpr int BUTTON_PCT_COUNT = sizeof(BUTTONS_PCT) / sizeof(BUTTONS_PCT[0]);

const ButtonPct* button_decode_pct(float pct) {
    for (int i = 0; i < BUTTON_PCT_COUNT; ++i) {
        if (pct >= BUTTONS_PCT[i].range.min && pct <= BUTTONS_PCT[i].range.max)
            return &BUTTONS_PCT[i];
    }
    return nullptr;
}

// ── State machine ─────────────────────────────────────────────────────────────

enum class BtnState { IDLE, PRESSED };

static float    s_idle             = 0.0f;
static BtnState s_state            = BtnState::IDLE;
static bool     s_seeded           = false;
static bool     s_went_idle        = false;
static int      s_confirm_count    = 0;
static float    s_last_press_pct   = 0.0f;

const ButtonPct* button_update(float vadc) {
    if (!s_seeded) {
        s_idle   = vadc;
        s_seeded = true;
        return nullptr;
    }

    float dev = (s_idle > 0.0f) ? absf(vadc - s_idle) / s_idle * 100.0f : 0.0f;

    if (s_state == BtnState::IDLE) {
        if (dev > PRESS_THRESHOLD_PCT) {
            if (++s_confirm_count >= CONFIRM_SAMPLES) {
                s_confirm_count  = 0;
                s_state          = BtnState::PRESSED;
                s_last_press_pct = (vadc / s_idle) * 100.0f;
                return button_decode_pct(s_last_press_pct);
            }
        } else {
            s_confirm_count = 0;
            // Asymmetric tracking: slow upward (don't chase press excursions),
            // fast downward (recover quickly after a press releases).
            float alpha = (vadc > s_idle) ? IDLE_ALPHA_UP : IDLE_ALPHA_DOWN;
            s_idle = s_idle * (1.0f - alpha) + vadc * alpha;
        }
        return nullptr;
    }

    // PRESSED — wait for voltage to return near idle before accepting next press.
    if (dev < RELEASE_THRESHOLD_PCT) {
        s_state     = BtnState::IDLE;
        s_went_idle = true;
    }
    return nullptr;
}

void button_seed_idle(float vadc) {
    s_idle   = vadc;
    s_seeded = true;
}

float button_last_press_pct() {
    float v          = s_last_press_pct;
    s_last_press_pct = 0.0f;
    return v;
}

float button_idle_vadc() { return s_idle; }

bool button_just_went_idle() {
    bool v    = s_went_idle;
    s_went_idle = false;
    return v;
}

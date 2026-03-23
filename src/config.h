#pragma once
#include <Arduino.h>

// ── Platform ───────────────────────────────────────────────────────────────
// Arduino Mega (ATmega2560, 5 V logic, 10-bit ADC)

// ── Pin assignments ────────────────────────────────────────────────────────
static constexpr int PIN_ADC        = A0;          // SWC signal (via 33k/10k divider)
static constexpr int PIN_ILLUM      = A1;          // Illumination gate output (bench) / input (car)
static constexpr int PIN_SWC_OUT    = 2;           // SWC output driver (2N7000 gate via 100 Ω)
static constexpr int PIN_BTN_SLEEP  = 4;           // Sleep toggle — wire button to GND, INPUT_PULLUP
static constexpr int PIN_LED_STATUS = LED_BUILTIN; // D13 built-in LED

// ── Serial ─────────────────────────────────────────────────────────────────
static constexpr long SERIAL_BAUD          = 9600;
static constexpr int  SERIAL_INIT_DELAY_MS = 1000;

// ── ADC ────────────────────────────────────────────────────────────────────
// The 33k/10k divider scales the SWC signal by 10/43 ≈ 0.233.
// ATmega2560 uses INTERNAL1V1 (1.1 V) for best resolution over the narrow
// ~0–0.77 V range produced by the divider.
static constexpr float ADC_REF_VOLTAGE = 5.0f;
static constexpr int   ADC_MAX_RAW     = 1023;

// ── Sampling ───────────────────────────────────────────────────────────────
static constexpr int SAMPLE_INTERVAL_MS = 100;  // 10 Hz
#pragma once
#include <Arduino.h>

#define FIRMWARE_VERSION "1.4.0"

// ── Platform ───────────────────────────────────────────────────────────────
// Arduino Mega (ATmega2560, 5 V logic, 10-bit ADC)

// ── Pin assignments ────────────────────────────────────────────────────────
static constexpr int PIN_ADC        = A1;          // SWC+ signal read
static constexpr int PIN_ADC_REF    = A1;          // SWC- reference (INPUT_PULLUP, ratiometric correction)
static constexpr int PIN_ILLUM      = A1;          // Illumination gate output (bench) / input (car)
static constexpr int PIN_SWC_OUT    = 2;           // SWC output driver (2N7000 gate via 100 Ω)
static constexpr int PIN_LED_STATUS = LED_BUILTIN; // D13 built-in LED

// ── Serial ─────────────────────────────────────────────────────────────────
static constexpr long SERIAL_BAUD          = 9600;
static constexpr int  SERIAL_INIT_DELAY_MS = 1000;

// ── ADC ────────────────────────────────────────────────────────────────────
// The 33k/10k divider scales the SWC signal by 10/43 ≈ 0.233.
// ADC reference is AVCC (5 V).
static constexpr float ADC_REF_VOLTAGE = 1.1f;
static constexpr int   ADC_MAX_RAW     = 1023;

// ── Sampling ───────────────────────────────────────────────────────────────
static constexpr int SAMPLE_INTERVAL_MS  = 100;  // 10 Hz
static constexpr int ADC_AVERAGE_SAMPLES = 21; // odd — clean median midpoint
static constexpr unsigned long STARTUP_DELAY_MS = 2000; // settle before first read
# CLAUDE.md — Corsa C SWC Interface · Arduino Nano Project

> Read this file fully before writing any code. It contains hardware facts that
> cannot be inferred from general knowledge. Getting them wrong produces
> circuits or firmware that silently misbehave on the bench and fail in the car.

---

## 1. Project Purpose

Interface the Opel Corsa C steering wheel audio controls (SWC) with an
**Arduino Nano (ATmega328P)**, so the steering wheel buttons can control a
headunit (volume up/down, next/prev track, source select) without cutting any
original wiring.

The Nano also receives live telemetry from a companion **Raspberry Pi Pico 2W**
(the injector piggyback) over UART and can display it via serial output.

---

## 2. Hardware Platform

| Item | Detail |
|------|--------|
| MCU | Arduino Nano (ATmega328P, 5 V, 16 MHz) |
| Language | C++ / Arduino framework (PlatformIO, `atmelavr`) |
| Host car | Opel Corsa C (2001–2006) |
| Companion | Raspberry Pi Pico 2W — communicates via UART |

The Nano runs at **5 V logic**. The ADC reference is set to the internal
**1.1 V** bandgap (`analogReference(INTERNAL)`) to maximise resolution over
the narrow SWC signal range that reaches the ADC pin (see section 3.3).

---

## 3. The SWC Circuit — Critical Hardware Facts

### 3.1 Resistor Ladder Architecture

The Corsa C SWC uses a **resistor ladder** on the steering wheel. Each button
connects a different resistance between the SWC signal wire and ground,
producing a distinct voltage that the headunit ADC reads.

**Approximate button voltages** (measured at the headunit connector, divider
loaded by headunit input impedance):

| Button | Approx. Resistance to GND | Approx. Voltage |
|--------|--------------------------|-----------------|
| Vol +  | ~1.5 kΩ                  | ~0.4 V |
| Vol −  | ~3.0 kΩ                  | ~0.7 V |
| Next   | ~6.0 kΩ                  | ~1.2 V |
| Prev   | ~12 kΩ                   | ~1.9 V |
| Source | ~22 kΩ                   | ~2.4 V |
| None   | open                     | ~3.3 V (pull-up) |

> **These values are approximations for initial firmware development.**
> Actual voltages must be measured on the bench with the real headunit
> connected, or with an equivalent load resistor. Do not hard-code thresholds
> without bench measurement — revise the ADC threshold table in `config.h`
> once real readings are available.

### 3.2 CRITICAL: The Illumination Bias Rail

**The SWC signal wire is biased by the headunit's `LED+` (illumination) rail,
not a standalone 5 V or 3.3 V rail.**

Consequences:

- The SWC signal pin carries **no meaningful voltage when the headunit
  illumination output is off** (i.e. ignition on but sidelights off).
- Attempting to read the ADC in this state will return floating/noise values.
- The firmware **must gate ADC reads** on whether the illumination rail is
  active.
- Detect illumination state by monitoring the `LED+` line via a voltage
  divider into a Nano digital input pin (A1).

Never assume the SWC line is always valid. Always check the illumination gate.

### 3.3 Voltage Divider — SWC Signal to Nano ADC

The SWC signal runs at headunit logic levels (roughly 0–3.3 V loaded, up to
~5 V unloaded). The Nano ADC absolute maximum is **5 V**, but we use the
**1.1 V internal reference** for resolution — so the signal must be scaled
below 1.1 V.

Use a **33 kΩ / 10 kΩ** voltage divider:

```
SWC_SIG ──── 33kΩ ──┬── Nano A0 (ADC)
                    │
                   10kΩ
                    │
                   GND
```

Divider ratio: 10 / (33 + 10) = **0.233**.

| Signal at SWC line | Voltage at A0 | Raw ADC (1.1 V ref, 10-bit) |
|--------------------|---------------|-----------------------------|
| 0.4 V (Vol+)       | ~0.093 V      | ~87                         |
| 0.7 V (Vol−)       | ~0.163 V      | ~152                        |
| 1.2 V (Next)       | ~0.279 V      | ~260                        |
| 1.9 V (Prev)       | ~0.442 V      | ~411                        |
| 2.4 V (Source)     | ~0.559 V      | ~520                        |
| 3.3 V (None)       | ~0.767 V      | ~714                        |

> These calculated counts assume ideal resistors and no headunit loading.
> Calibrate on the bench — the counts above are starting points only.

> The divider also loads the SWC line, affecting the voltage readings.
> Keep these exact resistor values when cross-referencing thresholds —
> changing them means re-deriving the entire table.

> Use **1% resistors** for the divider. 5% tolerance shifts thresholds
> enough to cause misidentification between adjacent buttons.

### 3.4 Output Stage — Simulating Headunit Button Presses

The Nano must **synthesise** SWC button presses to send to the headunit.
It does this by driving a 2N7000 N-channel MOSFET that pulls the SWC line
to ground through the appropriate button resistor. The headunit's own LED+
bias already provides the voltage source — only a low-side switch is needed.

Circuit (per Nano output pin → SWC line):

```
SWC line ──── R_button ──── 2N7000 (drain)
                             2N7000 (source) ── GND
Nano D2 ──── 100Ω ──── 2N7000 (gate)
```

D2 HIGH (5 V) → 2N7000 on → SWC line pulled through R_button to GND.
D2 LOW → 2N7000 off → SWC line floats back to LED+ (no button pressed).

**Part summary:**

| Part | Role |
|------|------|
| 2N7000 N-ch MOSFET | Low-side switch; gate threshold ≈ 0.8–3 V, driven by 5 V GPIO |
| 100 Ω | Gate series resistor — damps oscillation, limits gate charge current |
| R_button | Sets the resistance to GND that the headunit ADC reads as a specific button |

> The 2N7000 RDS(on) at VGS = 5 V is typically 5 Ω — negligible against
> the 1.5 kΩ–22 kΩ button resistors.

> Never drive the SWC line from a Nano GPIO pin directly. The line
> interacts with the 12 V illumination rail. Always use this MOSFET
> stage to isolate 5 V logic from the 12 V car circuit.

---

## 4. Pin Assignments

| Nano Pin | Direction | Function |
|----------|-----------|----------|
| D2 | OUT | SWC output driver (2N7000 gate via 100 Ω) |
| A0 | ADC IN | SWC signal read (via 33k/10k divider) |
| A1 | DIG IN/OUT | Illumination gate — INPUT in car, OUTPUT on bench |
| D4 | DIG IN | Sleep toggle button (external button to GND, INPUT_PULLUP) |
| D13 | OUT | Built-in status LED |
| D0 (RX) | UART RX | Telemetry from Pico |
| D1 (TX) | UART TX | Commands to Pico |

> D0/D1 are shared with the USB-Serial adapter on the Nano. Disconnect the
> Pico UART lines when uploading firmware via USB.

The Pico sends on **UART0 GPIO0 (TX) / GPIO1 (RX)** at 115200 baud.

---

## 5. Firmware Architecture

### 5.1 File Structure

```
src/
├── main.cpp          ← entry point, setup/loop
├── config.h          ← all tunable constants (thresholds, pins, baud)
├── adc_reader.h/.cpp ← ADC init, raw read, voltage conversion
├── illum_gate.h/.cpp ← drive/read illumination gate on A1
├── serial_cmd.h/.cpp ← non-blocking ON/OFF command parser
├── sleep_mode.h/.cpp ← button A debounce, sleep state
└── led_display.h/.cpp← D13 status LED patterns
```

### 5.2 Main Loop Responsibilities

```
loop:
  1. Poll sleep button (D4) — if toggled, print state and return early
  2. If sleeping: blink status LED, return
  3. Poll serial for ON/OFF commands → drive illumination gate
  4. Every SAMPLE_INTERVAL_MS: read ADC, print plotter line
```

### 5.3 Button Decoding (future — not yet implemented)

Define voltage thresholds as **midpoints** between adjacent raw ADC counts,
not exact expected values. ADC noise and resistor tolerance mean exact
matching will miss presses.

```cpp
// config.h — revise these after bench measurement (1.1 V ref, 10-bit)
// Midpoints between the calculated counts in section 3.3
static const int ADC_THRESHOLDS[][2] = {
    {120,  /* VOL_UP  */ },   // 0–120
    {206,  /* VOL_DOWN*/ },   // 120–206
    {335,  /* NEXT    */ },   // 206–335
    {465,  /* PREV    */ },   // 335–465
    {617,  /* SOURCE  */ },   // 465–617
    {1023, /* NONE    */ },   // 617–1023
};
```

### 5.4 Debounce

Require the same reading across **5 consecutive 100 ms samples** before
registering a press. Use a counter variable across loop iterations — do not
use `delay()` for debounce as it blocks serial polling.

### 5.5 Telemetry JSON Format

The Pico sends newline-delimited JSON at 115200 baud:

```json
{"vbat":12.40,"ch1":2840,"ch2":2835,"ch3":2842,"ch4":2838}
```

Fields:
- `vbat` — battery voltage (float, volts)
- `ch1`–`ch4` — injector pulse widths per cylinder (int, microseconds)

Use `Serial.available()` / non-blocking reads. Buffer lines manually and
parse on newline. Handle truncated lines gracefully.

> D0/D1 are used for both Pico UART and USB programming. Use `SoftwareSerial`
> on other pins if simultaneous USB monitoring and Pico comms are needed.

---

## 6. Status LED Behaviour (D13)

The Nano has no LED matrix. D13 (built-in LED) provides coarse state feedback.

| State | D13 |
|-------|-----|
| Illumination gate ON | Solid on |
| Illumination gate OFF | Off |
| Sleep mode | Blinks at 1 Hz |

---

## 7. Known Constraints and Non-Obvious Rules

1. **SWC signal is only valid with illumination on.** Gate all ADC reads
   behind the A1 illumination check. When illumination is off, serial output
   can still run — just don't attempt button decoding.

2. **Do not drive SWC output and read SWC input simultaneously.** When D2 is
   driving the 2N7000, A0 will read the driven voltage, not a button press.
   Multiplex: read first, then drive if needed, then release.

3. **The 2N7000 has a non-zero switching delay** (~10–100 ns). This is
   fast enough to be ignored for button simulation. Still hold the output
   for at least **80 ms** to ensure the headunit registers the press —
   that constraint comes from the headunit ADC polling rate, not the MOSFET.

4. **D0/D1 are shared with the USB adapter.** Disconnect the Pico UART
   wires before flashing. Consider `SoftwareSerial` if this becomes a
   recurring problem.

5. **Use 1% resistors** in the 33 kΩ / 10 kΩ divider. 5% tolerance shifts
   ADC thresholds enough to cause misidentification between adjacent buttons.

6. **ADC reference is 1.1 V internal.** Any voltage above ~1.1 V at A0 will
   clip at 1023. The divider (×0.233) ensures the worst-case SWC signal
   (~3.3 V) appears as ~0.77 V at A0 — safely below the clip point.

---

## 8. Out of Scope (Do Not Implement Unless Asked)

- CAN bus or K-Line ECU communication (handled by Pico, not Nano)
- Injector pulse scaling logic (Pico's domain)
- BLE / Bluetooth output to headunit (the SWC line is wired, not wireless)
- OTA firmware update

---

## 9. Testing Without the Car

For bench testing without a live SWC line:

- Drive A0 from a potentiometer off the Nano's 3.3 V pin (or a bench supply)
  through the same 33k/10k divider to simulate button voltages.
- Send `ON` over serial to drive A1 HIGH (illumination active) and enable
  ADC reads. Send `OFF` to simulate illumination absent.
- Use a USB serial terminal / Arduino Serial Plotter at 9600 baud to watch
  `Voltage_V` and `Illum` traces in real time.
- Verify each button position produces a distinct, stable ADC count before
  moving to in-car testing.

---

## 10. Related Project Context

This Nano is the **companion device** to the Pico 2W injector piggyback
(`main.cpp`, `config.h`, `injector_channel.h` in the parallel project). The
two devices communicate over UART; they do not share code or flash. Treat the
Pico as a black box that emits telemetry JSON — do not modify Pico firmware
from this project.

The Pico's UART0 TX (GPIO 0) → Nano D0 (RX).
The Pico's UART0 RX (GPIO 1) ← Nano D1 (TX).
Baud rate: 115200. No hardware flow control. 8N1.

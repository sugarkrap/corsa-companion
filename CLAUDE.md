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
the SWC signal range at A1.

---

## 3. The SWC Circuit

### 3.1 Resistor Ladder Architecture

The Corsa C SWC uses a **resistor ladder** on the steering wheel. Each button
connects a different resistance between the SWC signal wire and ground,
producing a distinct voltage read by the ADC.

There is **no external pull-up or voltage divider**. A1 is configured with
`INPUT_PULLUP` — the ATmega328P internal pull-up (~47 kΩ to VCC) is the only
bias. This forms a voltage divider between the internal pull-up and each
button's resistance to GND.

A **100 µF filter cap** is placed on A1 to GND to suppress EMI from the car
environment.

> Button ADC counts must be **calibrated on the bench** — the internal
> pull-up tolerance (~20–50 kΩ) and the actual button resistor values both
> affect the readings. Do not hard-code thresholds without measurement.

### 3.2 Button Detection — Ratiometric Approach

Detection is **ratiometric**: each sample is expressed as a percentage of
the current idle voltage rather than compared to absolute ADC counts. This
makes the algorithm self-calibrating and tolerant of slow supply drift.

Key parameters (in `button.cpp`):

| Parameter | Value | Meaning |
|-----------|-------|---------|
| `PRESS_THRESHOLD_PCT` | 3 % | Deviation from idle that opens a candidate press |
| `RELEASE_THRESHOLD_PCT` | 2 % | Deviation below which the button is considered released |
| `CONFIRM_SAMPLES` | 3 | Consecutive samples that must all exceed the threshold before a press is registered |
| `IDLE_ALPHA` | 0.35 | Low-pass weight for idle tracking (upward drift only) |

The idle reference tracks **upward only** — buttons can only pull the voltage
away from the baseline, never above it, so downward excursions are never
incorporated into the idle estimate.

A press is registered only after `CONFIRM_SAMPLES` consecutive readings all
deviate by more than `PRESS_THRESHOLD_PCT`. A single noisy sample cannot
fire a false press.

Button ranges are defined in `button.cpp` as `(vadc / idle_vadc) * 100` at
press time. **Calibrate these on the bench** — the ranges in source are
placeholders from initial measurements.

### 3.3 Output Stage — Simulating Headunit Button Presses

The Nano synthesises SWC button presses via a 2N7000 N-channel MOSFET that
pulls the SWC line to ground through the appropriate button resistor.

```
SWC line ──── R_button ──── 2N7000 (drain)
                             2N7000 (source) ── GND
Nano D2 ──── 100Ω ──── 2N7000 (gate)
```

D2 HIGH → 2N7000 on → SWC line pulled through R_button to GND.
D2 LOW  → 2N7000 off → SWC line returns to idle.

Hold the output for at least **80 ms** so the headunit ADC registers the press.

> Never drive the SWC line directly from a Nano GPIO — the line interacts
> with the car's 12 V circuitry. Always use the MOSFET stage.

---

## 4. Pin Assignments

| Nano Pin | Direction | Function |
|----------|-----------|----------|
| A1 | ADC IN | SWC signal read — `INPUT_PULLUP`, 100 µF cap to GND |
| D2 | OUT | SWC output driver (2N7000 gate via 100 Ω) |
| D13 | OUT | Built-in status LED |
| D0 (RX) | UART RX | Telemetry from Pico |
| D1 (TX) | UART TX | Commands to Pico |

> A1 is used for the ADC input. A0 is intentionally avoided — on an ATtiny85
> (the other supported target) A0 is shared with the RESET pin, which
> interferes with ISP flashing.

> D0/D1 are shared with the USB-Serial adapter. Disconnect the Pico UART
> wires before flashing via USB.

The Pico communicates on **UART0 GPIO0 (TX) / GPIO1 (RX)** at 115200 baud.

---

## 5. Firmware Architecture

### 5.1 Build Environments

| Environment | Board | Notes |
|-------------|-------|-------|
| `nano` | Arduino Nano (ATmega328P) | Full build — OLED, digital pot, TID display |
| `mega` | Arduino Mega 2560 | Full build |
| `uno` | Arduino Uno | Full build |
| `micro` | Arduino Micro | Full build |
| `nano_buttons_only` | Arduino Nano | `-D LIGHTWEIGHT` — ADC + button detection only |
| `attiny85_buttons_only` | ATtiny85 | `-D LIGHTWEIGHT` — ADC + button detection only |

The `LIGHTWEIGHT` flag strips all heavy peripherals (`oled_display`, `digital_pot`,
`tid_display`, `serial_cmd`) via `#ifndef LIGHTWEIGHT` guards in `main.cpp`.
The lightweight `build_src_filter` also excludes those `.cpp` files so the
Adafruit SSD1306/GFX libraries are never linked.

### 5.2 File Structure

```
src/
├── main.cpp            ← entry point, setup/loop
├── config.h            ← all tunable constants (thresholds, pins, baud, timing)
├── adc_reader.h/.cpp   ← ADC init, median read, single read
├── button.h/.cpp       ← ratiometric state machine, button table
├── oled_display.h/.cpp ← SSD1306 display (full build only)
├── serial_cmd.h/.cpp   ← non-blocking serial command parser (full build only)
├── tid_display.h/.cpp  ← TID scrolling display driver (full build only)
├── tid_writer.h/.cpp   ← TID write helpers (full build only)
├── digital_pot.h/.cpp  ← DS3502 I2C digital pot (full build only)
├── led_display.h/.cpp  ← D13 status LED patterns
└── illum_gate.h/.cpp   ← illumination gate (dead code, reserved)
```

### 5.3 Main Loop (current state)

```
setup:
  1. Serial.begin
  2. adc_init() — sets INTERNAL (1.1 V) reference, dummy read to settle
  3. Record s_ready_ms = millis() + STARTUP_DELAY_MS (2 s warmup)
  [full build only]
  4. s_pot.setup()

loop:
  [full build only]
  1. Poll serial for A/S/D/F keys → drive digital pot for 150 ms pulses
  2. Release pot to idle when pulse expires

  3. If millis() < s_ready_ms: return  ← startup hold-off
  4. Every SAMPLE_INTERVAL_MS (100 ms / 10 Hz):
       raw  = adc_read_raw_avg()  ← 21-sample median
       vadc = raw / 1023 * 1.1 V
       btn  = button_update(vadc) ← ratiometric state machine
       print Raw, V, VIdle, Pct, BTN
```

### 5.4 ADC Sampling

`adc_read_raw_avg()` collects `ADC_AVERAGE_SAMPLES` (21) readings and returns
the **median** — not the mean. This completely rejects spike outliers; a single
EMI-corrupted sample cannot shift the result as long as fewer than half the
samples are bad.

A 2-second startup delay (`STARTUP_DELAY_MS`) is enforced before the first
read to let the internal pull-up and filter cap settle and give `VIdle` a
clean initial seed.

### 5.5 Telemetry JSON Format (Pico → Nano)

The Pico sends newline-delimited JSON at 115200 baud:

```json
{"vbat":12.40,"ch1":2840,"ch2":2835,"ch3":2842,"ch4":2838}
```

Fields:
- `vbat` — battery voltage (float, volts)
- `ch1`–`ch4` — injector pulse widths per cylinder (int, microseconds)

---

## 6. Known Constraints and Non-Obvious Rules

1. **No external pull-up or divider.** The only bias is the ATmega328P internal
   pull-up. The 100 µF cap on A1 provides EMI filtering. Do not add external
   resistors without re-deriving the button voltage table.

2. **Do not drive SWC output and read SWC input simultaneously.** When D2 is
   driving the 2N7000, A1 will read the driven voltage, not a button press.
   Multiplex: read first, then drive if needed, then release.

3. **Button ranges in `button.cpp` are from initial bench calibration.**
   Re-measure after any circuit change. The ratiometric approach absorbs slow
   drift but not a change in the divider ratio.

4. **A1 is shared with `PIN_ADC_REF` and `PIN_ILLUM` in `config.h`** — those
   features are currently dead code. Do not activate them without resolving the
   pin conflict.

5. **D0/D1 are shared with the USB adapter.** Disconnect the Pico UART wires
   before flashing.

6. **ADC reference is 1.1 V internal.** Any voltage above ~1.1 V at A1 clips
   at Raw:1023. With the internal pull-up and open SWC line (no button) the
   pin is pulled to VCC (5 V) and will clip — this is expected and the idle
   tracker handles it correctly since clips only occur in the upward direction.

---

## 7. Out of Scope (Do Not Implement Unless Asked)

- CAN bus or K-Line ECU communication (handled by Pico, not Nano)
- Injector pulse scaling logic (Pico's domain)
- BLE / Bluetooth output to headunit (the SWC line is wired, not wireless)
- OTA firmware update

---

## 8. Testing Without the Car

- Connect the steering wheel SWC connector directly to A1 with the 100 µF cap
  in place. The internal pull-up provides bias.
- Use a USB serial terminal at 9600 baud to watch `Raw`, `V`, `VIdle`, `Pct`,
  and `BTN` output at 10 Hz.
- Press each button and record the stable `Pct` value — update the ranges in
  `button.cpp` accordingly.

---

## 9. Related Project Context

This Nano is the **companion device** to the Pico 2W injector piggyback. The
two devices communicate over UART; they do not share code or flash. Treat the
Pico as a black box that emits telemetry JSON — do not modify Pico firmware
from this project.

The Pico's UART0 TX (GPIO 0) → Nano D0 (RX).
The Pico's UART0 RX (GPIO 1) ← Nano D1 (TX).
Baud rate: 115200. No hardware flow control. 8N1.

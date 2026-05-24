#include "tid_writer.h"
#include <Arduino.h>

// ── Pin definitions ───────────────────────────────────────────────────────────
#define sda 6
#define scl 4
#define mrq 5
#define aa  7 // used to turn on the radio display, may not be used later down the line

// ── Timing ───────────────────────────────────────────────────────────────────
#define tid_delay  180
#define delay_300  300
#define delay_100  100

// ── Protocol constants ────────────────────────────────────────────────────────
#define TID_ADDR_BYTE 0x9B  // 10-char TID address byte, parity pre-baked
#define DATA_BYTES    10

// ── Debug ─────────────────────────────────────────────────────────────────────
#define TID_DEBUG false

// ── Internal helpers ──────────────────────────────────────────────────────────

static void tid_byte(byte data) {
    pinMode(scl, OUTPUT);
    for (uint8_t m = 0x80; m != 0; m >>= 1) {
        digitalWrite(sda, m & data);
        delayMicroseconds(10);
        digitalWrite(scl, HIGH);
        delayMicroseconds(tid_delay);
        digitalWrite(scl, LOW);
        delayMicroseconds(tid_delay);
    }
    delayMicroseconds(delay_300);
    pinMode(sda, INPUT);
    delayMicroseconds(tid_delay);
    pinMode(scl, INPUT);
    delayMicroseconds(delay_100);
    { unsigned long t = micros(); while (digitalRead(scl) == 0) { if (micros() - t > 5000) return; } }
    delayMicroseconds(delay_300);
    pinMode(scl, OUTPUT);
    digitalWrite(scl, LOW);
    while (digitalRead(sda) == 0);
    pinMode(sda, OUTPUT);
}

// Returns false if a pin wait timed out (display absent / unresponsive).
// handshakeWait=true: wait up to 60 s for the display to respond (useful on cold start).
static bool start_tid(bool handshakeWait = false) {
    unsigned long timeout = handshakeWait ? 60000000UL : 5000UL;
    unsigned long t;

#if TID_DEBUG
    if (handshakeWait) Serial.print(F("[TID] Waiting for handshake (up to 60 s)... "));
#endif

    pinMode(sda, INPUT);
    digitalWrite(mrq, LOW);
    delayMicroseconds(100);
    t = micros();
    while (digitalRead(sda) == 1) {
        if (micros() - t > timeout) {
#if TID_DEBUG
            Serial.println(F("[TID] Timeout waiting for SDA LOW"));
#endif
            digitalWrite(mrq, HIGH);
            return false;
        }
    }
#if TID_DEBUG
    Serial.print(F("SDA LOW at "));
    Serial.print(micros());
    Serial.print(F(" us — "));
#endif

    digitalWrite(mrq, HIGH);
    delayMicroseconds(100);
    t = micros();
    while (digitalRead(sda) == 0) {
        if (micros() - t > timeout) {
#if TID_DEBUG
            Serial.println(F("[TID] Timeout waiting for SDA HIGH"));
#endif
            return false;
        }
    }
#if TID_DEBUG
    Serial.print(F("SDA HIGH at "));
    Serial.print(micros());
    Serial.println(F(" us"));
#endif

    pinMode(sda, OUTPUT);
    pinMode(scl, OUTPUT);
    digitalWrite(sda, LOW);
    delayMicroseconds(100);
    digitalWrite(scl, LOW);
    delayMicroseconds(100);
    return true;
}

static void stop_tid() {
    digitalWrite(sda, LOW);
    delayMicroseconds(1000);
    digitalWrite(mrq, HIGH);
    delayMicroseconds(500);
    digitalWrite(scl, HIGH);
    delayMicroseconds(200);
    digitalWrite(sda, HIGH);
    delayMicroseconds(200);
}

// Takes a 7-bit value, shifts it into bits [7:1], sets bit 0 for odd parity.
static byte withParity(byte val) {
    byte b = val << 1;
    byte cnt = 0;
    for (byte tmp = b; tmp; tmp >>= 1) cnt += (tmp & 1);
    if (cnt % 2 == 0) b |= 0x01;
    return b;
}

// ── Symbol state ──────────────────────────────────────────────────────────────
static byte s_sym1 = 0;
static byte s_sym2 = 0;

// ── Public API ────────────────────────────────────────────────────────────────

void tid_set_symbol(byte symbo, bool on) {
    byte rw = on ? 1 : 0;
    switch (symbo) {
        case 9:  bitWrite(s_sym1, 0, rw); break;
        case 3:  bitWrite(s_sym1, 1, rw); break;
        case 5:  bitWrite(s_sym1, 3, rw); break;
        case 10: bitWrite(s_sym1, 4, rw); break;
        case 7:  bitWrite(s_sym1, 5, rw); break;
        case 11: bitWrite(s_sym1, 6, rw); break;
        case 8:  bitWrite(s_sym2, 2, rw); break;
        case 4:  bitWrite(s_sym2, 3, rw); break;
        case 2:  bitWrite(s_sym2, 4, rw); break;
        case 1:  bitWrite(s_sym2, 5, rw); break;
        case 6:  bitWrite(s_sym2, 6, rw); break;
    }
}

void tid_writer_init(bool useAA = false) {
    if (useAA) {
        pinMode(aa, OUTPUT);
        digitalWrite(aa, LOW);
    }

    pinMode(mrq, OUTPUT);
    pinMode(sda, OUTPUT);
    pinMode(scl, OUTPUT);

    digitalWrite(mrq, HIGH);
    digitalWrite(sda, HIGH);
    digitalWrite(scl, HIGH);

    if (useAA) {
        digitalWrite(aa, HIGH);
    }

    delay(500);
}

void sendTID(const char* text, bool handshakeWait) {
    if (!start_tid(handshakeWait)) {
#if TID_DEBUG
        Serial.println(F("[TID] Transmission aborted: handshake failed"));
#endif
        return;
    }

    tid_byte(TID_ADDR_BYTE);
    digitalWrite(mrq, LOW);

    tid_byte(withParity(s_sym1));
    tid_byte(withParity(s_sym2));
    tid_byte(withParity(s_sym2));  // sym2 sent twice, matches reference

    for (uint8_t i = 0; i < DATA_BYTES; i++) {
        char c = (text[i] != '\0') ? text[i] : ' ';
        tid_byte(withParity((byte)c));
    }

    stop_tid();
#if TID_DEBUG
    Serial.print(F("[TID] Sent: \""));
    for (uint8_t i = 0; i < DATA_BYTES; i++) Serial.print((text[i] != '\0') ? text[i] : ' ');
    Serial.println(F("\""));
#endif
}
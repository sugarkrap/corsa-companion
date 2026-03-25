#include "tid_writer.h"

// ── Pin definitions ───────────────────────────────────────────────────────────
#define sda 4
#define scl 5
#define mrq 6
#define aa  7 // used to turn on the radio display, may not be used later down the line

// ── Timing ───────────────────────────────────────────────────────────────────
#define tid_delay  150
#define delay_300  150
#define delay_100  100

// ── Protocol constants ────────────────────────────────────────────────────────
#define TID_ADDR   0x4D   // 10-digit TID (Corsa C / Astra G), raw — parity added below
#define DATA_BYTES 10

// ── Internal helpers ──────────────────────────────────────────────────────────

static void tid_byte(byte data) {
    pinMode(scl, OUTPUT);
    for (uint8_t m = 0x80; m != 0; m >>= 1) {
        digitalWrite(sda, m & data);
        delayMicroseconds(5);
        digitalWrite(scl, HIGH);
        delayMicroseconds(50);
        digitalWrite(scl, LOW);
        delayMicroseconds(50);
    }
    delayMicroseconds(delay_300);
    pinMode(sda, INPUT);
    delayMicroseconds(tid_delay);
    pinMode(scl, INPUT);
    delayMicroseconds(delay_100);
    while (digitalRead(scl) == 0);
    delayMicroseconds(delay_300);
    pinMode(scl, OUTPUT);
    digitalWrite(scl, LOW);
    while (digitalRead(sda) == 0);
    pinMode(sda, OUTPUT);
}

static void start_tid() {
    pinMode(sda, INPUT);
    digitalWrite(mrq, LOW);
    delayMicroseconds(100);
    while (digitalRead(sda) == 1);
    digitalWrite(mrq, HIGH);
    delayMicroseconds(100);
    while (digitalRead(sda) == 0);
    pinMode(sda, OUTPUT);
    pinMode(scl, OUTPUT);
    digitalWrite(sda, LOW);
    delayMicroseconds(100);
    digitalWrite(scl, LOW);
    delayMicroseconds(100);
}

static void stop_tid() {
    digitalWrite(sda, LOW);
    delayMicroseconds(100);
    digitalWrite(mrq, HIGH);
    delayMicroseconds(500);
    digitalWrite(scl, HIGH);
    delayMicroseconds(100);
    digitalWrite(sda, HIGH);
    delayMicroseconds(100);
}

// Takes a 7-bit value, shifts it into bits [7:1], sets bit 0 for odd parity.
static byte withParity(byte val) {
    byte b = val << 1;
    byte cnt = 0;
    for (byte tmp = b; tmp; tmp >>= 1) cnt += (tmp & 1);
    if (cnt % 2 == 0) b |= 0x01;
    return b;
}

// ── Public API ────────────────────────────────────────────────────────────────

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

void sendTID(byte sym1, byte sym2, byte sym3, const char* text) {
    start_tid();

    tid_byte(withParity(TID_ADDR >> 1));

    tid_byte(withParity(sym1));
    tid_byte(withParity(sym2));
    tid_byte(withParity(sym3));

    for (uint8_t i = 0; i < DATA_BYTES; i++) {
        char c = (text[i] != '\0') ? text[i] : ' ';
        tid_byte(withParity((byte)c));
    }

    stop_tid();
}
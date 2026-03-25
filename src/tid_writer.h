#pragma once
#include <Arduino.h>

// TID (Text Information Display) writer for Corsa C / Astra G instrument cluster.
//
// Pin usage — NOTE: sda=D4 conflicts with PIN_BTN_SLEEP; re-wire sleep button
// to another free pin if both modules are active simultaneously.
//   D4 — SDA
//   D5 — SCL
//   D6 — MRQ

// Call once in setup().
void tid_writer_init(bool useAA);

// Send a full TID frame.
//   sym1/sym2/sym3 — 7-bit symbol bytes (odd parity added automatically)
//   text           — exactly 10 characters; shorter strings are space-padded
void sendTID(byte sym1, byte sym2, byte sym3, const char* text);
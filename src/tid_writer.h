#pragma once
#include <Arduino.h>

// ── Symbol numbers ────────────────────────────────────────────────────────────
#define TID_SYM_CASSETTE  1
#define TID_SYM_ADM       2
#define TID_SYM_AS        3
#define TID_SYM_CR        4
// 5 — no visible symbol
#define TID_SYM_CD        6
#define TID_SYM_ADS       7
#define TID_SYM_CPS       8
#define TID_SYM_BRACKET   9
#define TID_SYM_TP        10
#define TID_SYM_TICK      11

// TID (Text Information Display) writer for Corsa C / Astra G instrument cluster.
//
// Pin usage
//   D4 — SCL
//   D5 — MRQ
//   D6 — SDA

// Call once in setup().
void tid_writer_init(bool useAA);

// Set or clear a symbol on the TID display (persists across sendTID calls).
// Symbol numbers match the reference library (1–11).
void tid_set_symbol(byte symbo, bool on);

// Send a full TID frame using current symbol state.
//   text          — exactly 10 characters; shorter strings are space-padded
//   handshakeWait — if true, wait up to 60 s for display to respond (cold start)
void sendTID(const char* text, bool handshakeWait = false);
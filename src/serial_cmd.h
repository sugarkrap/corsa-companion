#pragma once
#include <stdint.h>

// Non-blocking serial command reader.
// Call serial_cmd_poll() each loop iteration.
// Returns one of the CMD_* values when a complete command is received.

enum SerialCmd {
    CMD_NONE = 0,
    CMD_ON,
    CMD_OFF,
    CMD_SYM,  // SYM <1-11> <0|1>
};

void      serial_cmd_init();
SerialCmd serial_cmd_poll();

// Valid only immediately after serial_cmd_poll() returns CMD_SYM.
uint8_t serial_cmd_sym_number();
bool serial_cmd_sym_state();
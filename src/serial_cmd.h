#pragma once

// Non-blocking serial command reader.
// Call serial_cmd_poll() each loop iteration.
// Returns one of the CMD_* values when a complete command is received.

enum SerialCmd {
    CMD_NONE = 0,
    CMD_ON,
    CMD_OFF,
};

void      serial_cmd_init();
SerialCmd serial_cmd_poll();
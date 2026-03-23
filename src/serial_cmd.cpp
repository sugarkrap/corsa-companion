#include "serial_cmd.h"
#include "config.h"
#include <string.h>

static char s_buf[8];
static int  s_len = 0;

void serial_cmd_init() {
    s_len = 0;
}

SerialCmd serial_cmd_poll() {
    while (Serial.available()) {
        char c = (char)Serial.read();

        if (c == '\n' || c == '\r') {
            s_buf[s_len] = '\0';
            SerialCmd result = CMD_NONE;

            if (strcmp(s_buf, "ON") == 0)       result = CMD_ON;
            else if (strcmp(s_buf, "OFF") == 0) result = CMD_OFF;

            s_len = 0;
            if (result != CMD_NONE) return result;

        } else if (s_len < (int)sizeof(s_buf) - 1) {
            s_buf[s_len++] = c;
        }
    }
    return CMD_NONE;
}
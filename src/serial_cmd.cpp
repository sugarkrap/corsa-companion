#include "serial_cmd.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>

static char    s_buf[12];
static int     s_len = 0;
static uint8_t s_sym_number = 0;
static bool    s_sym_state  = false;

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
            else if (strncmp(s_buf, "SYM ", 4) == 0) {
                uint8_t num = (uint8_t)atoi(s_buf + 4);
                const char* space = strchr(s_buf + 4, ' ');
                if (space && num >= 1 && num <= 11) {
                    s_sym_number = num;
                    s_sym_state  = (*(space + 1) == '1');
                    result = CMD_SYM;
                }
            }

            s_len = 0;
            if (result != CMD_NONE) return result;

        } else if (s_len < (int)sizeof(s_buf) - 1) {
            s_buf[s_len++] = c;
        }
    }
    return CMD_NONE;
}

uint8_t serial_cmd_sym_number() { return s_sym_number; }
bool    serial_cmd_sym_state()  { return s_sym_state; }
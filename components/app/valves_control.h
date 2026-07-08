#ifndef VALVES_CONTROL_H
#define VALVES_CONTROL_H

#include "esp_err.h"
#include <stdint.h>

void handle_valve_cmd(uint8_t cmd, int time_ms);
void handle_valve_cmd_angle(uint8_t cmd, int time_ms, int angle);

#endif
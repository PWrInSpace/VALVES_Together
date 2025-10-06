#ifndef VALVES_CONTROL_H
#define VALVES_CONTROL_H

#include <stdint.h>
#include "esp_err.h"

void chandle_valve_cmd(uint8_t cmd, int time_ms);

#endif
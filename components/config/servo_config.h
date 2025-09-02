#pragma once
#include "valve_board_config.h"
#include "servo_control.h"

extern Servo_t servos[SERVO_COUNT];

esp_err_t init_multiple_servos();
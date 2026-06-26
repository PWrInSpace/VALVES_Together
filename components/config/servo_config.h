#pragma once
#include "servo_control.h"
#include "valve_board_config.h"

extern Servo_t servos[SERVO_COUNT];

esp_err_t init_multiple_servos();
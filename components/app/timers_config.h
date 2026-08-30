#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#include "esp_err.h"
#include "system_timer.h"

esp_err_t timers_init(void);
esp_err_t start_test_timer(void);

#endif
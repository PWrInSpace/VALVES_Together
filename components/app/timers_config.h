#ifndef TIMERS_CONFIG_H
#define TIMERS_CONFIG_H

#include "system_timer.h"
#include "esp_err.h"

esp_err_t timers_init(void);
esp_err_t start_test_timer(void);

#endif
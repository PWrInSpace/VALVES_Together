#ifndef TEMPERATURE_TASK_H
#define TEMPERATURE_TASK_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
esp_err_t temperature_task_init(void);
esp_err_t temperature_task_deinit(void);
void temperature_task(void *arg);
#endif // TEMPERATURE_TASK_H
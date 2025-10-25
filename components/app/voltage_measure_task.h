
#ifndef VOLTAGE_MEASURE_TASK_H
#define VOLTAGE_MEASURE_TASK_H

#include "esp_err.h"



esp_err_t voltage_task_init(void);
esp_err_t voltage_task_deinit(void);
void voltage_task(void *arg);

#endif //VOLTAGE_MEASURE_TASK_H
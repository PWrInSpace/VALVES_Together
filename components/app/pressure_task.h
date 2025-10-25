
#ifndef PRESSURE_TASK_H
#define PRESSURE_TASK_H

#include "esp_err.h"



esp_err_t pressure_task_init(void);
esp_err_t pressure_task_deinit(void);
void pressure_task(void *arg);

#endif //PRESSURE_TASK_H
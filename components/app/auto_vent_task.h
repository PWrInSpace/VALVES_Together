

#ifndef AUTO_VENT_TASK_H
#define AUTO_VENT_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "valve_board_config.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef SOL_N20_SERVO_ETH_CONFIG

#define AUTO_VENT_OPEN_TIME_MS 3000 // open vent time
#define AUTO_VENT_TRIGGERED_STATUS_MS                                          \
  1000 // time to show that the vent was triggered
#define AUTO_VENT_DEBOUNCE_TIME_MS 5000 // debounce time

extern volatile bool is_auto_vent_active;
extern volatile bool is_triggered;
extern float auto_vent_pressure;
extern SemaphoreHandle_t auto_vent_pressure_mutex;
extern TaskHandle_t auto_vent_task_handle;

void auto_vent_task(void *arg);
bool set_auto_vent_pressure(float pressure);
bool get_auto_vent_pressure(float *pressure);
bool set_auto_vent_on(float pressure);
bool set_auto_vent_off();

bool run_auto_vent_task();

#endif

#endif /* AUTO_VENT_TASK_H */
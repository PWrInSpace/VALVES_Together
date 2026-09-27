#ifndef SD_TASK_H
#define SD_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "sdkconfig.h"
#define MOUNT_POINT "/sdcard"

#ifndef CONFIG_SD_FILE_PREFIX
#define SD_FILE_PREFIX "log"
#else
#define SD_FILE_PREFIX CONFIG_SD_FILE_PREFIX
#endif

extern TaskHandle_t sd_task;
extern volatile bool new_filename_flag;
esp_err_t sd_task_init(void);

bool run_sd_task();
bool run_update_data_task();

#endif
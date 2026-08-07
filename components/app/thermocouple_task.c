#include "thermocouple_task.h"

#include "BoardData.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "max31856.h"
#include "thermocouple_config.h"

#define TAG "THERMOCOUPLE_TASK"
#define THERMOCOUPLE_PERIOD_MS 1000
#define BOARDDATA_MUTEX_TIMEOUT_MS 10

TaskHandle_t thermocouple_task_handle = NULL;

void thermocouple_task(void *arg) {
  (void)arg;

  while (1) {
    float temperatures[THERMOCOUPLE_COUNT];
    for (int i = 0; i < THERMOCOUPLE_COUNT; i++) {
      thermocouple_read_fault(&thermocouple_devices[i], false);
      float temp_c = thermocouple_read_temperature(&thermocouple_devices[i]);
      temperatures[i] = temp_c;
    }
    set_boardData_temperatures(temperatures,
                               pdMS_TO_TICKS(BOARDDATA_MUTEX_TIMEOUT_MS));
    vTaskDelay(pdMS_TO_TICKS(THERMOCOUPLE_PERIOD_MS));
  }
}

bool run_thermocouple_task(void) {
  ESP_LOGI(TAG, "Starting thermocouple measurement task");
  BaseType_t result = xTaskCreate(thermocouple_task, "thermocouple_task", 8192,
                                  NULL, 5, &thermocouple_task_handle);

  if (result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create thermocouple_task");
    return false;
  }

  return true;
}

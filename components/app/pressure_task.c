#include "BoardData.h"
#include "igniter_task.h"
#include "pressure_driver.h"

TaskHandle_t press_task_handle = NULL;
#define TAG "pressure_TASK"
void press_task(void *arg) {
  while (1) {

    if (xSemaphoreTake(BoardDataSemaphore, portMAX_DELAY) == pdTRUE) {
      pressure_driver_read_pressures(&pressure_driver_config,
                                     boardData.pressure);
      // ESP_LOGI(TAG, "Pressure readings: %f, %f, %f", boardData.pressure[0],
      // boardData.pressure[1], boardData.pressure[2]);
      xSemaphoreGive(BoardDataSemaphore);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

bool run_pressure_task() {
  ESP_LOGI("IGNITER_TASK", "Starting igniter update data task");
  BaseType_t result =
      xTaskCreate(press_task, "press_task", 8192, NULL, 5, &press_task_handle);

  if (result != pdPASS) {
    ESP_LOGE("pressure_TASK", "Failed to create igniter_task");
    return false;
  }

  return true;
}

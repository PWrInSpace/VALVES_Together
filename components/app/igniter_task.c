#include "igniter_task.h"
#include "BoardData.h"
#include "igniter_driver.h"

TaskHandle_t igniter_task_handle = NULL;
#define TAG "IGNITER_TASK"
void igniter_task(void *arg) {
  igniter_continuity_t continuity;
  igniter_status_t status;
  while (1) {

    status = igniter_check_continuity(igniter_cfg, &continuity);

    if (status != IGNITER_OK) {
      ESP_LOGE(TAG, "Igniter continuity check failed with status %d", status);
    }

    BoardData_t new_bd;
    if (get_board_data(&new_bd, portMAX_DELAY) == ESP_OK) {
      new_bd.dump_valve_arm = (igniter_cfg->state == IGNITER_STATE_ARMED) ? 1 : 0;
      new_bd.dump_valve_cont = (continuity == IGNITER_CONTINUITY_OK) ? 1 : 0;

      set_board_data(new_bd, portMAX_DELAY);
    }

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

bool run_igniter_task() {
  ESP_LOGI("IGNITER_TASK", "Starting igniter update data task");
  BaseType_t result = xTaskCreate(igniter_task, "igniter_task", 8192, NULL, 5,
                                  &igniter_task_handle);

  if (result != pdPASS) {
    ESP_LOGE("IGNITER_TASK", "Failed to create igniter_task");
    return false;
  }

  return true;
}

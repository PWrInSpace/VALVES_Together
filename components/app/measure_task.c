#include "BoardData.h"
#include "buzzer_task.h"
#include "ltc4162.h"
#include "mcu_i2c_config.h"
#include "pressure_driver.h"

TaskHandle_t pressure_task_handle = NULL;
TaskHandle_t charger_task_handle = NULL;

#define TAG "MEASURE_TASK"
#define MEASURE_PERIOD_MS 10
#define I2C_MUTEX_TIMEOUT_MS 10
#define BOARDDATA_MUTEX_TIMEOUT_MS 10

void pressure_task(void *arg) {
  while (1) {
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) ==
        pdTRUE) {
      float temp_pressures[4];

      pressure_driver_read_pressures(&pressure_driver_config, temp_pressures,
                                     5);
      set_boardData_pressures(temp_pressures, BOARDDATA_MUTEX_TIMEOUT_MS);

      xSemaphoreGive(mcu_i2c_mutex);
    }

    vTaskDelay(pdMS_TO_TICKS(MEASURE_PERIOD_MS));
  }
}

void charger_task(void *arg) {
  uint16_t system_status_raw = UINT16_MAX;

  while (1) {
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) ==
        pdTRUE) {
      ltc4162_charger_data_t charger_data = {0};

      vTaskDelay(pdMS_TO_TICKS(10));

      if (read_charger_data(&charger_data) == ESP_OK) {
        ltc4162_charger_data_t new_charger_data = {
            .vbat = charger_data.vbat,
            .vin = charger_data.vin,
            .ibat = charger_data.ibat,
            .iin = charger_data.iin,
            .die_temp = charger_data.die_temp,
            .vout = charger_data.vout,
            .charger_status = charger_data.charger_status,
            .charger_state = charger_data.charger_state,
            .system_status = charger_data.system_status};
        xSemaphoreGive(mcu_i2c_mutex);

        // ESP_LOGI(TAG, "New system status raw: 0x%04X",
        // new_charger_data.system_status); ESP_LOGI(TAG, "System status raw:
        // 0x%04X", system_status_raw);

        if (system_status_raw == 0x00A1 &&
            new_charger_data.system_status == 0x0067) {
          play_buzzer_sound(SOUND_CHARGER_CONNECTED);
          ESP_LOGI(TAG, "Charger connected");
        } else if (system_status_raw == 0x0067 &&
                   (new_charger_data.system_status == 0x0023 ||
                    new_charger_data.system_status == 0x00A3 ||
                    new_charger_data.system_status == 0x00A1)) {
          // after disconnect, system status can be
          // 0x0023 or 0x00A3 per one frame then 0x00A1
          play_buzzer_sound(SOUND_CHARGER_DISCONNECTED);
          ESP_LOGI(TAG, "Charger disconnected");
        }

        system_status_raw = new_charger_data.system_status;

        BoardData_t new_bd;
        if (get_board_data(&new_bd, BOARDDATA_MUTEX_TIMEOUT_MS) == ESP_OK) {
          new_bd.is_charging = charger_data.charger_state;
          new_bd.chargerData = new_charger_data;

          set_board_data(new_bd, BOARDDATA_MUTEX_TIMEOUT_MS);
        }
      }

      // ltc4162_debug_monitor();
    }

    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

bool run_measure_task() {
  ESP_LOGI(TAG, "Starting measure update data task");
  BaseType_t result = xTaskCreate(pressure_task, "pressure_task", 8192, NULL, 5,
                                  &pressure_task_handle);

  if (result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create pressure_task");
    return false;
  }

  result = xTaskCreate(charger_task, "charger_task", 8192, NULL, 5,
                       &charger_task_handle);
  if (result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create charger_task");
    return false;
  }

  return true;
}

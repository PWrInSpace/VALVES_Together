#include "BoardData.h"
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
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) == pdTRUE) {
      float temp_pressures[4];

      pressure_driver_read_pressures(&pressure_driver_config, temp_pressures, 5);
      set_boardData_pressures(temp_pressures, BOARDDATA_MUTEX_TIMEOUT_MS);

      xSemaphoreGive(mcu_i2c_mutex);
    }

    vTaskDelay(pdMS_TO_TICKS(MEASURE_PERIOD_MS));
  }
}

void charger_task(void *arg) {
  while (1) {
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) ==
        pdTRUE) {
      ltc4162_charger_data_t charger_data = {0};

      if (mcu_i2c_deinit() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinit I2C");
        xSemaphoreGive(mcu_i2c_mutex);
        continue;
      }

      if (mcu_i2c_init_with_pins(SDA_GPIO_ALT, SCL_GPIO_ALT) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init I2C with alt pins");
        xSemaphoreGive(mcu_i2c_mutex);
        continue;
      }

      vTaskDelay(pdMS_TO_TICKS(10));

      if (read_charger_data(&charger_data) == ESP_OK) {
        ChargerData_t new_charger_data = {
          .vbat = charger_data.vbat,
          .vin = charger_data.vin,
          .ibat = charger_data.ibat,
          .iin = charger_data.iin,
          .die_temp = charger_data.die_temp,
          .vout = charger_data.vout,
          .charger_status = charger_data.charger_status,
          .charger_state = charger_data.charger_state
        };

        BoardData_t new_bd;
        if (get_board_data(&new_bd, BOARDDATA_MUTEX_TIMEOUT_MS) == ESP_OK) {
          new_bd.is_charging = charger_data.charger_state;
          new_bd.chargerData = new_charger_data;

          set_board_data(new_bd, BOARDDATA_MUTEX_TIMEOUT_MS);
        }
      }

      // ltc4162_debug_monitor();

      if (mcu_i2c_deinit() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deinit I2C");
        xSemaphoreGive(mcu_i2c_mutex);
        continue;
      }

      if (mcu_i2c_init_with_pins(SDA_GPIO, SCL_GPIO) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init I2C with default pins");
        xSemaphoreGive(mcu_i2c_mutex);
        continue;
      }
      xSemaphoreGive(mcu_i2c_mutex);
    }

    vTaskDelay(pdMS_TO_TICKS(1000));
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

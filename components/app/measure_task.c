#include "BoardData.h"
#include "buzzer.h"
#include "buzzer_task.h"
#include "esp_log.h"
#include "ltc4162.h"
#include "max31856.h"
#include "mcu_i2c_config.h"
#include "pressure_driver.h"
#include "thermocouple_config.h"

TaskHandle_t pressure_task_handle = NULL;
TaskHandle_t charger_task_handle = NULL;
TaskHandle_t thermocouple_task_handle = NULL;

#define TAG "MEASURE_TASK"
#define MEASURE_PERIOD_MS 10
#define I2C_MUTEX_TIMEOUT_MS 250
#define BOARDDATA_MUTEX_TIMEOUT_MS 10
#define THERMOCOUPLE_PERIOD_MS 100
#define BOARDDATA_MUTEX_TIMEOUT_MS 10

static void pressure_task(void *arg) {
  while (1) {
    float temp_pressures[4];
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) ==
        pdTRUE) {
      pressure_driver_read_pressures(&pressure_driver_config, temp_pressures);
      xSemaphoreGive(mcu_i2c_mutex);
      set_boardData_pressures(temp_pressures, BOARDDATA_MUTEX_TIMEOUT_MS);
    }
  }
}

static void charger_task(void *arg) {
  uint16_t system_status_raw = UINT16_MAX;
  bool charging_notification_sent = false;

  vTaskDelay(pdMS_TO_TICKS(3000));

  while (1) {
    ltc4162_charger_data_t charger_data = {0};
    if (xSemaphoreTake(mcu_i2c_mutex, pdMS_TO_TICKS(I2C_MUTEX_TIMEOUT_MS)) ==
        pdTRUE) {

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
            .system_status = charger_data.system_status,
            .vin_supply = 0.0f,
            .vext_supply = 0.0f};
        xSemaphoreGive(mcu_i2c_mutex);

        // ESP_LOGI(TAG, "New system status raw: 0x%04X",
        // new_charger_data.system_status); ESP_LOGI(TAG, "System status raw:
        // 0x%04X", system_status_raw);

        // TODO do naprawy
        // if (system_status_raw == 0x00A1 &&
        //     new_charger_data.system_status == 0x0067) {
        //   play_buzzer_sound(SOUND_CHARGER_CONNECTED);
        //   ESP_LOGI(TAG, "Charger connected");
        // } else if (system_status_raw == 0x0067 &&
        //            (new_charger_data.system_status == 0x0023 ||
        //             new_charger_data.system_status == 0x00A3 ||
        //             new_charger_data.system_status == 0x00A1)) {
        //   // after disconnect, system status can be
        //   // 0x0023 or 0x00A3 per one frame then 0x00A1
        //   play_buzzer_sound(SOUND_CHARGER_DISCONNECTED);
        //   ESP_LOGI(TAG, "Charger disconnected");
        // }

        system_status_raw = new_charger_data.system_status;

        switch (system_status_raw) {
        case 0x00A3:
          new_charger_data.vin_supply = new_charger_data.vin;
          new_charger_data.vext_supply = new_charger_data.vout;
          break;
        case 0x00A1:
        case 0x0023:
          new_charger_data.vin_supply = new_charger_data.vout;
          break;
        case 0x0067:
          new_charger_data.vext_supply = new_charger_data.vout;
          new_charger_data.vin_supply = new_charger_data.vbat;
          break;
        default:
          break;
        }

        const bool has_external = new_charger_data.vext_supply > 1.0f;
        const bool is_charging_now =
            has_external && ((!charging_notification_sent &&
                              new_charger_data.vext_supply >
                                  new_charger_data.vin_supply + 0.10f) ||
                             (charging_notification_sent &&
                              new_charger_data.vext_supply >
                                  new_charger_data.vin_supply - 0.10f));

        if (is_charging_now && !charging_notification_sent) {
          play_buzzer_sound(SOUND_CHARGER_CONNECTED);
        }

        charging_notification_sent = is_charging_now;

        BoardData_t new_bd;
        if (get_board_data(&new_bd, BOARDDATA_MUTEX_TIMEOUT_MS) == ESP_OK) {
          new_bd.is_charging = is_charging_now;
          new_bd.chargerData = new_charger_data;

          set_board_data(new_bd, BOARDDATA_MUTEX_TIMEOUT_MS);
        }
      } else {
        xSemaphoreGive(mcu_i2c_mutex);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

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

#ifdef SERVO_N20_CONFIG
  ESP_LOGI(TAG, "Starting thermocouple measurement task");
  result = xTaskCreate(thermocouple_task, "thermocouple_task", 8192, NULL, 5,
                       &thermocouple_task_handle);

  if (result != pdPASS) {
    ESP_LOGE(TAG, "Failed to create thermocouple_task");
    return false;
  }
#endif

  return true;
}

///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 12.02.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//

#include "pressure_driver.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mcu_i2c_config.h"
#include "valve_board_config.h"

#define TAG "PRESSURE_DRIVER"

static ads1115_struct_t ads1115_config = {
    ._i2c_write = _mcu_i2c_write,
    ._i2c_read = _mcu_i2c_read,
    .i2c_address = 0x49,
};

bool calibration_mode;

pressure_driver_struct_t pressure_driver_config =
    PRESSURE_DRIVER_TANWA_CONFIG(&ads1115_config);

pressure_driver_status_t
pressure_driver_init(pressure_driver_struct_t *pressure_driver) {

#ifdef SOL_N20_SERVO_ETH_CONFIG
  // prawy
  pressure_driver_config.sensors[0].pressure_min = 0.0f;
  pressure_driver_config.sensors[0].pressure_max = 350.0f;
  pressure_driver_config.sensors[0].voltage_min = 0.371f;
  pressure_driver_config.sensors[0].voltage_max = 4.096f;

  pressure_driver_config.sensors[1].pressure_min = 0.0f;
  pressure_driver_config.sensors[1].pressure_max = 350.0f;
  pressure_driver_config.sensors[1].voltage_min = 0.0f;
  pressure_driver_config.sensors[1].voltage_max = 4.096f;

  // lewy
  pressure_driver_config.sensors[2].pressure_min = 0.0f;
  pressure_driver_config.sensors[2].pressure_max = 60.0f;
  pressure_driver_config.sensors[2].voltage_min = 0.373f; // 0.01f --> 0.04Bar
  pressure_driver_config.sensors[2].voltage_max = 2.961840f;

  //Środkowy
  pressure_driver_config.sensors[3].pressure_min = 0.0f;
  pressure_driver_config.sensors[3].pressure_max = 60.0f;
  pressure_driver_config.sensors[3].voltage_min = 0.3685f;
  pressure_driver_config.sensors[3].voltage_max = 2.952090f;

#elif defined(SOL_ETH_CONFIG)
  // prawy
  pressure_driver_config.sensors[0].pressure_min = 0.0f; // +0.1f --> -0.2Bar
  pressure_driver_config.sensors[0].pressure_max = 80.0f;
  pressure_driver_config.sensors[0].voltage_min = 0.357f;
  pressure_driver_config.sensors[0].voltage_max = 1.031032f;

  pressure_driver_config.sensors[1].pressure_min = 0.0f;
  pressure_driver_config.sensors[1].pressure_max = 350.0f;
  pressure_driver_config.sensors[1].voltage_min = 0.0f;
  pressure_driver_config.sensors[1].voltage_max = 4.096f;

  // lewy
  pressure_driver_config.sensors[2].pressure_min = 0.0f;
  pressure_driver_config.sensors[2].pressure_max = 60.0f;
  pressure_driver_config.sensors[2].voltage_min = 0.371f;
  pressure_driver_config.sensors[2].voltage_max = 0.884f;

  //Środkowy
  pressure_driver_config.sensors[3].pressure_min = 0.0f;
  pressure_driver_config.sensors[3].pressure_max = 60.0f;
  pressure_driver_config.sensors[3].voltage_min = 0.371f;
  pressure_driver_config.sensors[3].voltage_max = 0.884f;

#elif defined(SERVO_N20_CONFIG)
  // prawy
  pressure_driver_config.sensors[0].pressure_min = 0.0f;
  pressure_driver_config.sensors[0].pressure_max = 350.0f;
  pressure_driver_config.sensors[0].voltage_min = 0.371f;
  pressure_driver_config.sensors[0].voltage_max = 4.096f;

  pressure_driver_config.sensors[1].pressure_min = 0.0f;
  pressure_driver_config.sensors[1].pressure_max = 350.0f;
  pressure_driver_config.sensors[1].voltage_min = 0.0f;
  pressure_driver_config.sensors[1].voltage_max = 4.096f;

  // lewy
  pressure_driver_config.sensors[2].pressure_min = 0.0f; // +0.1f --> -0.2Bar
  pressure_driver_config.sensors[2].pressure_max = 80.0f;
  pressure_driver_config.sensors[2].voltage_min = 0.366f;
  pressure_driver_config.sensors[2].voltage_max = 1.051032F;

  //Środkowy
  pressure_driver_config.sensors[3].pressure_min = 0.0f;
  pressure_driver_config.sensors[3].pressure_max = 60.0f;
  pressure_driver_config.sensors[3].voltage_min = 0.371f;
  pressure_driver_config.sensors[3].voltage_max = 0.884f;

#else
  // prawy
  pressure_driver_config.sensors[0].pressure_min = 0.0f;
  pressure_driver_config.sensors[0].pressure_max = 350.0f;
  pressure_driver_config.sensors[0].voltage_min = 0.371f;
  pressure_driver_config.sensors[0].voltage_max = 4.096f;

  pressure_driver_config.sensors[1].pressure_min = 0.0f;
  pressure_driver_config.sensors[1].pressure_max = 350.0f;
  pressure_driver_config.sensors[1].voltage_min = 0.0f;
  pressure_driver_config.sensors[1].voltage_max = 4.096f;

  // lewy
  pressure_driver_config.sensors[2].pressure_min = 0.0f;
  pressure_driver_config.sensors[2].pressure_max = 60.0f;
  pressure_driver_config.sensors[2].voltage_min = 0.371f;
  pressure_driver_config.sensors[2].voltage_max = 0.884f;

  //Środkowy
  pressure_driver_config.sensors[3].pressure_min = 0.0f;
  pressure_driver_config.sensors[3].pressure_max = 60.0f;
  pressure_driver_config.sensors[3].voltage_min = 0.371f;
  pressure_driver_config.sensors[3].voltage_max = 0.884f;

#endif

  calibration_mode = false;

  // pressure_driver_config.sensors[0].raw_min = 0;
  // pressure_driver_config.sensors[0].raw_max = 32767;
  // pressure_driver_config.sensors[0].pressure_min = 0.0f;
  // pressure_driver_config.sensors[0].pressure_max = 350.0f;

  // pressure_driver_config.sensors[1].raw_min = 0;
  // pressure_driver_config.sensors[1].raw_max = 32767;
  // pressure_driver_config.sensors[1].pressure_min = 0.0f;
  // pressure_driver_config.sensors[1].pressure_max = 350.0f;

  // pressure_driver_config.sensors[2].raw_min = 0;
  // pressure_driver_config.sensors[2].raw_max = 32767;
  // pressure_driver_config.sensors[2].pressure_min = 0.0f;
  // pressure_driver_config.sensors[2].pressure_max = 350.0f;

  // //UNUSED
  // pressure_driver_config.sensors[3].raw_min = 0;
  // pressure_driver_config.sensors[3].raw_max = 32767;
  // pressure_driver_config.sensors[3].pressure_min = 0.0f;
  // pressure_driver_config.sensors[3].pressure_max = 350.0f;

  if (pressure_driver == NULL) {
    return PRESSURE_DRIVER_FAIL;
  }
  ads1115_mode_t mode;
  ads1115_data_rate_t rate;
  ads1115_set_mode(pressure_driver->ads1115, ADS1115_MODE_CONTINUOUS);
  vTaskDelay(pdMS_TO_TICKS(50));
  ads1115_set_data_rate(pressure_driver->ads1115, ADS1115_DATA_RATE_860);
  vTaskDelay(pdMS_TO_TICKS(50));
  ads1115_set_gain(pressure_driver->ads1115, ADS1115_GAIN_4V096);
  vTaskDelay(pdMS_TO_TICKS(50));
  ads1115_get_mode(pressure_driver->ads1115, &mode);
  printf("MODE = %d", (bool)mode);
  ads1115_get_data_rate(pressure_driver->ads1115, &rate);
  printf("RATE = %d", (uint8_t)rate);
  // vTaskDelay(pdMS_TO_TICKS(2500));

  return PRESSURE_DRIVER_OK;
}

// unused
//  pressure_driver_status_t
//  pressure_driver_set_min_pressure(pressure_driver_struct_t *pressure_driver,
//  pressure_driver_sensor_t sensor, float pressure) {
//      if (pressure_driver == NULL) {
//          return PRESSURE_DRIVER_FAIL;
//      }

//     pressure_driver->sensors[sensor].pressure_min = pressure;

//     return PRESSURE_DRIVER_OK;
// }

// pressure_driver_status_t
// pressure_driver_set_max_pressure(pressure_driver_struct_t *pressure_driver,
// pressure_driver_sensor_t sensor, float pressure) {
//     if (pressure_driver == NULL) {
//         return PRESSURE_DRIVER_FAIL;
//     }

//     pressure_driver->sensors[sensor].pressure_max = pressure;

//     return PRESSURE_DRIVER_OK;
// }

// pressure_driver_status_t
// pressure_driver_set_min_voltage(pressure_driver_struct_t *pressure_driver,
// pressure_driver_sensor_t sensor, float voltage) {
//     if (pressure_driver == NULL) {
//         return PRESSURE_DRIVER_FAIL;
//     }

//     pressure_driver->sensors[sensor].voltage_min = voltage;

//     return PRESSURE_DRIVER_OK;
// }

// pressure_driver_status_t
// pressure_driver_set_max_voltage(pressure_driver_struct_t *pressure_driver,
// pressure_driver_sensor_t sensor, float voltage) {
//     if (pressure_driver == NULL) {
//         return PRESSURE_DRIVER_FAIL;
//     }

//     pressure_driver->sensors[sensor].voltage_max = voltage;

//     return PRESSURE_DRIVER_OK;
// }

pressure_driver_status_t
pressure_driver_read_voltage(pressure_driver_struct_t *pressure_driver,
                             pressure_driver_sensor_t sensor, float *voltage) {
  if (pressure_driver == NULL) {
    return PRESSURE_DRIVER_FAIL;
  }

  int16_t raw;
  ads1115_mux_t mux;
  ads1115_get_value(pressure_driver->ads1115, &raw);
  ads1115_set_input_mux(pressure_driver->ads1115,
                        pressure_driver->sensors[sensor].adc_pin);
  vTaskDelay(pdMS_TO_TICKS(5));
  ads1115_get_value(pressure_driver->ads1115, &raw);
  *voltage = ads1115_gain_values[ADS1115_GAIN_4V096] / ADS1115_MAX_VALUE * raw;
  return PRESSURE_DRIVER_OK;
}

// float pressure_driver_read_pressure(pressure_driver_struct_t
// *pressure_driver, pressure_driver_sensor_t sensor) {
//     if (pressure_driver == NULL) {
//         return PRESSURE_DRIVER_FAIL;
//     }

//     float voltage;
//     float pressure;
//     pressure_driver_read_voltage(pressure_driver, sensor, &voltage);
//     pressure = (voltage - pressure_driver->sensors[sensor].voltage_min) *
//     (pressure_driver->sensors[sensor].pressure_max -
//     pressure_driver->sensors[sensor].pressure_min) /
//     (pressure_driver->sensors[sensor].voltage_max -
//     pressure_driver->sensors[sensor].voltage_min) +
//     pressure_driver->sensors[sensor].pressure_min;

//     return pressure;
// }

pressure_driver_status_t
pressure_driver_read_pressures(pressure_driver_struct_t *pressure_driver,
                               float *pressure) {
  if (pressure_driver == NULL || pressure == NULL) {
    ESP_LOGE(TAG, "Invalid argument in pressure_driver_read_pressures");
    return PRESSURE_DRIVER_FAIL;
  }

  float voltage[PRESSURE_DRIVER_SENSOR_COUNT];
  for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) {
    // Read voltage for the current sensor
    pressure_driver_status_t status =
        pressure_driver_read_voltage(pressure_driver, i, &voltage[i]);
    if (status != PRESSURE_DRIVER_OK) {
      ESP_LOGE(TAG, "Failed to read voltage for sensor %d", i);
      return status;
    }

    // Calculate pressure for the current sensor
    pressure[i] = (voltage[i] - pressure_driver->sensors[i].voltage_min) *
                      (pressure_driver->sensors[i].pressure_max -
                       pressure_driver->sensors[i].pressure_min) /
                      (pressure_driver->sensors[i].voltage_max -
                       pressure_driver->sensors[i].voltage_min) +
                  pressure_driver->sensors[i].pressure_min;

    if (calibration_mode) {
      // SENSOR 2 lewy
      // SENSOR 3 środkowy
      // SENSOR 0 prawy

      ESP_LOGI(TAG, "Sensor %d, voltage: %.6f V, pressure: %.6f", i, voltage[i],
               pressure[i]);
      vTaskDelay(pdMS_TO_TICKS(400));
    }
  }

  if (calibration_mode) {
    ESP_LOGI(TAG, "\n\n------------------------------\n\n");
  }

  return PRESSURE_DRIVER_OK;
}
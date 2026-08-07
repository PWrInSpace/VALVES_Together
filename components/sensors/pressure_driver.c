///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 12.02.2024 by Michał Kos
/// Modified: 23.05.2026 by Mateusz Kluczka
///
///===-----------------------------------------------------------------------------------------===//

#include "pressure_driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include "mcu_i2c_config.h"
#include "valve_board_config.h"

#define TAG "PRESSURE_DRIVER"

static ads1115_struct_t ads1115_config = {
    ._i2c_write = _mcu_i2c_write,
    ._i2c_read = _mcu_i2c_read,
    .i2c_address = 0x49,
};

pressure_driver_struct_t pressure_driver_config =
    PRESSURE_DRIVER_TANWA_CONFIG(&ads1115_config);

// pressure_driver_status_t pressure_driver_init(pressure_driver_struct_t
// *pressure_driver) {

// #ifdef SOL_N20_SERVO_ETH_CONFIG
//   // prawy
//   pressure_driver_config.sensors[0].pressure_min = 0.0f;
//   pressure_driver_config.sensors[0].pressure_max = 350.0f;
//   pressure_driver_config.sensors[0].voltage_min = 0.371f;
//   pressure_driver_config.sensors[0].voltage_max = 4.096f;

//   pressure_driver_config.sensors[1].pressure_min = 0.0f;
//   pressure_driver_config.sensors[1].pressure_max = 350.0f;
//   pressure_driver_config.sensors[1].voltage_min = 0.0f;
//   pressure_driver_config.sensors[1].voltage_max = 4.096f;

//   // lewy
//   pressure_driver_config.sensors[2].pressure_min = 0.0f;
//   pressure_driver_config.sensors[2].pressure_max = 80.0f;
//   pressure_driver_config.sensors[2].voltage_min = 0.375386f;
//   pressure_driver_config.sensors[2].voltage_max = 1.067033f;

//   //Środkowy
//   pressure_driver_config.sensors[3].pressure_min = 0.0f;
//   pressure_driver_config.sensors[3].pressure_max = 80.0f;
//   pressure_driver_config.sensors[3].voltage_min = 0.375761f;
//   pressure_driver_config.sensors[3].voltage_max = 1.063658f;

// #elif defined(SOL_ETH_CONFIG)
//   // prawy
//   pressure_driver_config.sensors[0].pressure_min = 0.0f;
//   pressure_driver_config.sensors[0].pressure_max = 80.0f;
//   pressure_driver_config.sensors[0].voltage_min = 0.368136f;
//   pressure_driver_config.sensors[0].voltage_max = 1.035782f;

//   pressure_driver_config.sensors[1].pressure_min = 0.0f;
//   pressure_driver_config.sensors[1].pressure_max = 350.0f;
//   pressure_driver_config.sensors[1].voltage_min = 0.0f;
//   pressure_driver_config.sensors[1].voltage_max = 4.096f;

//   // lewy
//   pressure_driver_config.sensors[2].pressure_min = 0.0f;
//   pressure_driver_config.sensors[2].pressure_max = 60.0f;
//   pressure_driver_config.sensors[2].voltage_min = 0.371f;
//   pressure_driver_config.sensors[2].voltage_max = 0.884f;

//   //Środkowy
//   pressure_driver_config.sensors[3].pressure_min = 0.0f;
//   pressure_driver_config.sensors[3].pressure_max = 60.0f;
//   pressure_driver_config.sensors[3].voltage_min = 0.371f;
//   pressure_driver_config.sensors[3].voltage_max = 0.884f;

// #elif defined(SOL_N2_CONFIG)
//   // prawy
//   pressure_driver_config.sensors[0].pressure_min = 0.0f;
//   pressure_driver_config.sensors[0].pressure_max = 350.0f;
//   pressure_driver_config.sensors[0].voltage_min = 0.371f;
//   pressure_driver_config.sensors[0].voltage_max = 4.096f;

//   pressure_driver_config.sensors[1].pressure_min = 0.0f;
//   pressure_driver_config.sensors[1].pressure_max = 350.0f;
//   pressure_driver_config.sensors[1].voltage_min = 0.0f;
//   pressure_driver_config.sensors[1].voltage_max = 4.096f;

//   // lewy
//   pressure_driver_config.sensors[2].pressure_min = 0.0f;
//   pressure_driver_config.sensors[2].pressure_max = 80.0f;
//   pressure_driver_config.sensors[2].voltage_min = 0.366f;
//   pressure_driver_config.sensors[2].voltage_max = 1.051032f;

//   //Środkowy
//   pressure_driver_config.sensors[3].pressure_min = 0.0f;
//   pressure_driver_config.sensors[3].pressure_max = 60.0f;
//   pressure_driver_config.sensors[3].voltage_min = 0.371f;
//   pressure_driver_config.sensors[3].voltage_max = 0.884f;

// #else
//   // prawy
//   pressure_driver_config.sensors[0].pressure_min = 0.0f;
//   pressure_driver_config.sensors[0].pressure_max = 350.0f;
//   pressure_driver_config.sensors[0].voltage_min = 0.371f;
//   pressure_driver_config.sensors[0].voltage_max = 4.096f;

//   pressure_driver_config.sensors[1].pressure_min = 0.0f;
//   pressure_driver_config.sensors[1].pressure_max = 350.0f;
//   pressure_driver_config.sensors[1].voltage_min = 0.0f;
//   pressure_driver_config.sensors[1].voltage_max = 4.096f;

//   // lewy
//   pressure_driver_config.sensors[2].pressure_min = 0.0f;
//   pressure_driver_config.sensors[2].pressure_max = 60.0f;
//   pressure_driver_config.sensors[2].voltage_min = 0.371f;
//   pressure_driver_config.sensors[2].voltage_max = 0.884f;

//   //Środkowy
//   pressure_driver_config.sensors[3].pressure_min = 0.0f;
//   pressure_driver_config.sensors[3].pressure_max = 60.0f;
//   pressure_driver_config.sensors[3].voltage_min = 0.371f;
//   pressure_driver_config.sensors[3].voltage_max = 0.884f;

// #endif

//   calibration_mode = false;

//   // pressure_driver_config.sensors[0].raw_min = 0;
//   // pressure_driver_config.sensors[0].raw_max = 32767;
//   // pressure_driver_config.sensors[0].pressure_min = 0.0f;
//   // pressure_driver_config.sensors[0].pressure_max = 350.0f;

//   // pressure_driver_config.sensors[1].raw_min = 0;
//   // pressure_driver_config.sensors[1].raw_max = 32767;
//   // pressure_driver_config.sensors[1].pressure_min = 0.0f;
//   // pressure_driver_config.sensors[1].pressure_max = 350.0f;

//   // pressure_driver_config.sensors[2].raw_min = 0;
//   // pressure_driver_config.sensors[2].raw_max = 32767;
//   // pressure_driver_config.sensors[2].pressure_min = 0.0f;
//   // pressure_driver_config.sensors[2].pressure_max = 350.0f;

//   // //UNUSED
//   // pressure_driver_config.sensors[3].raw_min = 0;
//   // pressure_driver_config.sensors[3].raw_max = 32767;
//   // pressure_driver_config.sensors[3].pressure_min = 0.0f;
//   // pressure_driver_config.sensors[3].pressure_max = 350.0f;

//   if (pressure_driver == NULL) {
//     return PRESSURE_DRIVER_FAIL;
//   }
//   ads1115_mode_t mode;
//   ads1115_data_rate_t rate;
//   ads1115_set_mode(pressure_driver->ads1115, ADS1115_MODE_CONTINUOUS);
//   vTaskDelay(pdMS_TO_TICKS(50));
//   ads1115_set_data_rate(pressure_driver->ads1115, ADS1115_DATA_RATE_860);
//   vTaskDelay(pdMS_TO_TICKS(50));
//   ads1115_set_gain(pressure_driver->ads1115, ADS1115_GAIN_4V096);
//   vTaskDelay(pdMS_TO_TICKS(50));
//   ads1115_get_mode(pressure_driver->ads1115, &mode);
//   printf("MODE = %d", (bool)mode);
//   ads1115_get_data_rate(pressure_driver->ads1115, &rate);
//   printf("RATE = %d", (uint8_t)rate);
//   // vTaskDelay(pdMS_TO_TICKS(2500));

//   return PRESSURE_DRIVER_OK;
// }

pressure_driver_status_t
pressure_driver_init(pressure_driver_struct_t *pressure_driver) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

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

pressure_driver_status_t
pressure_driver_set_zero_voltage(pressure_driver_struct_t *pressure_driver,
                                 pressure_driver_sensor_t sensor,
                                 float voltage) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

  pressure_driver->sensors[sensor].calibr_cfg.voltage_zero = voltage;

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
pressure_driver_set_1_voltage(pressure_driver_struct_t *pressure_driver,
                              pressure_driver_sensor_t sensor, float voltage) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

  pressure_driver->sensors[sensor].calibr_cfg.voltage_1 = voltage;

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
pressure_driver_set_1_pressure(pressure_driver_struct_t *pressure_driver,
                               pressure_driver_sensor_t sensor,
                               float pressure) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

  pressure_driver->sensors[sensor].calibr_cfg.pressure_1 = pressure;

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
pressure_driver_read_voltage(pressure_driver_struct_t *pressure_driver,
                             pressure_driver_sensor_t sensor, float *voltage) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

  int16_t raw;

  ads1115_get_value(pressure_driver->ads1115, &raw);
  ads1115_set_input_mux(pressure_driver->ads1115,
                        pressure_driver->sensors[sensor].adc_pin);
  vTaskDelay(pdMS_TO_TICKS(5));
  ads1115_get_value(pressure_driver->ads1115, &raw);
  *voltage = ads1115_gain_values[ADS1115_GAIN_4V096] / ADS1115_MAX_VALUE * raw;

  return PRESSURE_DRIVER_OK;
}

float pressure_driver_read_pressure(pressure_driver_struct_t *pressure_driver,
                                    pressure_driver_sensor_t sensor) {
  if (pressure_driver == NULL)
    return PRESSURE_DRIVER_FAIL;

  float voltage;
  float pressure;
  pressure_driver_read_voltage(pressure_driver, sensor, &voltage);
  pressure =
      (voltage - pressure_driver->sensors[sensor].calibr_cfg.voltage_zero) *
      (pressure_driver->sensors[sensor].calibr_cfg.pressure_1) /
      (pressure_driver->sensors[sensor].calibr_cfg.voltage_1 -
       pressure_driver->sensors[sensor].calibr_cfg.voltage_zero);

  return pressure;
}

pressure_driver_status_t
pressure_driver_read_pressures(pressure_driver_struct_t *pressure_driver,
                               float *pressures, size_t measure_samples) {
  if (pressure_driver == NULL || pressures == NULL || measure_samples == 0) {
    ESP_LOGE(TAG, "Invalid argument in pressure_driver_read_pressures");
    return PRESSURE_DRIVER_FAIL;
  }

  for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) {
    // Read voltage for the current sensor
    float v_raw;
    pressure_driver_status_t status = measure_voltage_mean_from_samples(
        pressure_driver, i, measure_samples, &v_raw);
    if (status != PRESSURE_DRIVER_OK) {
      ESP_LOGE(TAG, "Failed to read voltage for sensor %d", i);
      return status;
    }

    float v0 = pressure_driver->sensors[i].calibr_cfg.voltage_zero;
    float v1 = pressure_driver->sensors[i].calibr_cfg.voltage_1;
    float p1 = pressure_driver->sensors[i].calibr_cfg.pressure_1;

    // Avoid dividing by zero
    if (fabsf(v1 - v0) < 0.0001f) {
      pressures[i] = 0.0f;
    } else {
      pressures[i] = (v_raw - v0) * (p1 / (v1 - v0));
    }

    if (pressures[i] < 0)
      pressures[i] = 0;
  }

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
measure_voltage_mean_from_samples(pressure_driver_struct_t *pressure_driver,
                                  pressure_driver_sensor_t sensor,
                                  size_t samples, float *out_voltage) {
  if (pressure_driver == NULL || out_voltage == NULL || samples == 0) {
    ESP_LOGE(TAG, "Invalid argument in measure_voltage_mean_from_samples");
    return PRESSURE_DRIVER_FAIL;
  }

  float voltage, voltageSum = 0;
  ;
  for (int i = 0; i < samples; i++) {
    pressure_driver_status_t ret =
        pressure_driver_read_voltage(pressure_driver, sensor, &voltage);
    if (ret != PRESSURE_DRIVER_OK)
      return ret;
    voltageSum = voltage;
  }

  *out_voltage = (float)(voltageSum / samples);
  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
calibrate_pressure_sensor(pressure_driver_struct_t *pressure_driver,
                          pressure_driver_sensor_t sensor, float pressure,
                          float *measured_volt, size_t measure_samples) {
  if (pressure_driver == NULL || measured_volt == NULL ||
      measure_samples == 0) {
    ESP_LOGE(TAG, "Invalid argument in calibrate_pressure_sensor");
    return PRESSURE_DRIVER_FAIL;
  }

  if (measure_voltage_mean_from_samples(pressure_driver, sensor,
                                        measure_samples,
                                        measured_volt) != PRESSURE_DRIVER_OK) {
    return PRESSURE_DRIVER_FAIL;
  }

  // no need to check return type
  pressure_driver_set_1_voltage(pressure_driver, sensor, *measured_volt);
  pressure_driver_set_1_pressure(pressure_driver, sensor, pressure);

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
tare_pressure_sensor(pressure_driver_struct_t *pressure_driver,
                     pressure_driver_sensor_t sensor, float *measured_volt,
                     size_t measure_samples) {
  if (pressure_driver == NULL || measured_volt == NULL ||
      measure_samples == 0) {
    ESP_LOGE(TAG, "Invalid argument in tare_pressure_sensor");
    return PRESSURE_DRIVER_FAIL;
  }

  if (measure_voltage_mean_from_samples(pressure_driver, sensor,
                                        measure_samples,
                                        measured_volt) != PRESSURE_DRIVER_OK) {
    return PRESSURE_DRIVER_FAIL;
  }

  // no need to check return type
  pressure_driver_set_zero_voltage(pressure_driver, sensor, *measured_volt);

  return PRESSURE_DRIVER_OK;
}

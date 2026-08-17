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
#include <stddef.h>

#define TAG "PRESSURE_DRIVER"
#define PRESSURE_DRIVER_MAX_SAMPLES 1

static ads1115_struct_t ads1115_config = {
    ._i2c_write = _mcu_i2c_write,
    ._i2c_read = _mcu_i2c_read,
    .i2c_address = 0x49,
};

pressure_driver_struct_t pressure_driver_config =
    PRESSURE_DRIVER_TANWA_CONFIG(&ads1115_config);

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
  esp_rom_delay_us(2000);
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
                               float *pressures) {
  if (pressure_driver == NULL || pressures == NULL) {
    ESP_LOGE(TAG, "Invalid argument in pressure_driver_read_pressures");
    return PRESSURE_DRIVER_FAIL;
  }

  // for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) { //press 0 -
  // termistor jest unused
  for (int i = 1; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) {
    // Read voltage for the current sensor
    float v_raw;
    pressure_driver_status_t status =
        measure_voltage_mean_from_samples(pressure_driver, i, &v_raw);
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
  }

  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
measure_voltage_mean_from_samples(pressure_driver_struct_t *pressure_driver,
                                  pressure_driver_sensor_t sensor,
                                  float *out_voltage) {
  if (pressure_driver == NULL || out_voltage == NULL) {
    ESP_LOGE(TAG, "Invalid argument in measure_voltage_mean_from_samples");
    return PRESSURE_DRIVER_FAIL;
  }

  float voltage = 0.0f;

  float voltage_samples[PRESSURE_DRIVER_MAX_SAMPLES];
  size_t samples = PRESSURE_DRIVER_MAX_SAMPLES;
  for (int i = 0; i < samples; i++) {
    pressure_driver_status_t ret =
        pressure_driver_read_voltage(pressure_driver, sensor, &voltage);
    if (ret != PRESSURE_DRIVER_OK) {
      return ret;
    }
    voltage_samples[i] = voltage;
  }

  for (size_t i = 1; i < samples; i++) {
    float key = voltage_samples[i];
    size_t j = i;
    while (j > 0 && voltage_samples[j - 1] > key) {
      voltage_samples[j] = voltage_samples[j - 1];
      j--;
    }
    voltage_samples[j] = key;
  }

  *out_voltage =
      (samples % 2 == 0)
          ? (voltage_samples[samples / 2 - 1] + voltage_samples[samples / 2]) /
                2.0f
          : voltage_samples[samples / 2];
  return PRESSURE_DRIVER_OK;
}

pressure_driver_status_t
calibrate_pressure_sensor(pressure_driver_struct_t *pressure_driver,
                          pressure_driver_sensor_t sensor, float pressure,
                          float *measured_volt) {
  if (pressure_driver == NULL || measured_volt == NULL) {
    ESP_LOGE(TAG, "Invalid argument in calibrate_pressure_sensor");
    return PRESSURE_DRIVER_FAIL;
  }

  if (measure_voltage_mean_from_samples(pressure_driver, sensor,

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
                     pressure_driver_sensor_t sensor, float *measured_volt) {
  if (pressure_driver == NULL || measured_volt == NULL) {
    ESP_LOGE(TAG, "Invalid argument in tare_pressure_sensor");
    return PRESSURE_DRIVER_FAIL;
  }

  if (measure_voltage_mean_from_samples(pressure_driver, sensor,

                                        measured_volt) != PRESSURE_DRIVER_OK) {
    return PRESSURE_DRIVER_FAIL;
  }

  // no need to check return type
  pressure_driver_set_zero_voltage(pressure_driver, sensor, *measured_volt);

  return PRESSURE_DRIVER_OK;
}

void apply_pressure_calibration(void) {
  data_config_t cfg;
  if (flash_get_runtime_config(&cfg) != ESP_OK) {
    ESP_LOGW(TAG, "Cannot read config, keeping default pressure calibration");
    return;
  }
  const pressure_sensor_calibration_config_t
      calibr[PRESSURE_DRIVER_SENSOR_COUNT] = {
          {.voltage_zero = cfg.press_calibr.sensor_0_volt_0,
           .voltage_1 = cfg.press_calibr.sensor_0_volt_1,
           .pressure_1 = cfg.press_calibr.sensor_0_press_1},
          {.voltage_zero = cfg.press_calibr.sensor_1_volt_0,
           .voltage_1 = cfg.press_calibr.sensor_1_volt_1,
           .pressure_1 = cfg.press_calibr.sensor_1_press_1},
          {.voltage_zero = cfg.press_calibr.sensor_2_volt_0,
           .voltage_1 = cfg.press_calibr.sensor_2_volt_1,
           .pressure_1 = cfg.press_calibr.sensor_2_press_1},
          {.voltage_zero = cfg.press_calibr.sensor_3_volt_0,
           .voltage_1 = cfg.press_calibr.sensor_3_volt_1,
           .pressure_1 = cfg.press_calibr.sensor_3_press_1},
      };
  for (int i = 0; i < PRESSURE_DRIVER_SENSOR_COUNT; i++) {
    if (fabsf(calibr[i].voltage_1 - calibr[i].voltage_zero) < 0.0001f) {
      ESP_LOGW(TAG, "Sensor %d calibration invalid, keeping defaults", i);
      continue;
    }
    pressure_driver_config.sensors[i].calibr_cfg = calibr[i];
  }
}
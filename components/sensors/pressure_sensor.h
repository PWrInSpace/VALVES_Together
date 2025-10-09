/**
 * @file pressure_sensor.h
 * @author Michal Kos
 * @brief Trafag 8252 pressure sensor reading
 * @version 0.1
 * @date 2022-08-27
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef PRESSURE_H
#define PRESSURE_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"


#define PRESSURE_R1 15000.0
#define PRESSURE_R2 30000.0
#define PRESSURE1_ADC_CHANNEL 3U
#define PRESSURE2_ADC_CHANNEL 4U

// ADC channel attenuation - can be changed
#define ADC_ATTEN_PRESSURE ADC_ATTEN_DB_12

#define PRESSURE_SENSOR_INIT(X, Y, Z, Q)                            \
  {                                                                 \
    .adc_channel = X, .cali_enable = Q, .adc_raw = 0, .voltage = 0, \
    .adc_handle = Y, .adc_cali_handle = Z                           \
  }


typedef struct {
    adc_channel_t adc_channel;
    uint32_t adc_raw;
    uint32_t voltage;
    adc_oneshot_unit_handle_t *adc_handle;
    adc_cali_handle_t *adc_cali_handle;
    bool cali_enable;
} Pressure_Sensor_t;

typedef struct {
  uint8_t Initialized;
  // 1ADCs
  adc_oneshot_unit_handle_t mADC_1;
  adc_cali_handle_t mADC_1_cali;
  bool mADC_1_cali_enabled;

  //2ADCs
  adc_oneshot_unit_handle_t mADC_2;
  adc_cali_handle_t mADC_2_cali;
  bool mADC_2_cali_enabled;


  // Pressure sensor
  Pressure_Sensor_t mPressure1;
  Pressure_Sensor_t mPressure2;
} Pressure_Manager_t;

bool pressure_sensors_init();

uint32_t get_pressure(Pressure_Sensor_t *sensor_ptr);

extern Pressure_Manager_t pressure_manager;
#endif  // PRESSURE_H

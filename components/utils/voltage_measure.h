#ifndef VOLTAGE_MEASURE_HH
#define VOLTAGE_MEASURE_HH

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "pressure_sensor.h"

#define DIV_MULTIPLIER_VOL 8.5034
#define VOLTAGE_ADC_CHANNEL 7U //TODO zmmienic na 1 gdy bedzie plytka

/**************************  INIT *********************************************/

#define VOLTAGE_MEASURE_INIT(X, Y, Z, Q)                            \
  {                                                                 \
    .adc_channel = X, .cali_enable = Q, .adc_raw = 0, .voltage = 0, \
    .adc_handle = Y, .adc_cali_handle = Z                           \
  }

/**************************  PUBLIC VARIABLES  ********************************/

/**************************  CODE *********************************************/

typedef struct Voltage_Measure {
    adc_channel_t adc_channel;
    uint32_t adc_raw;
    uint32_t voltage;
    adc_oneshot_unit_handle_t *adc_handle;
    adc_cali_handle_t *adc_cali_handle;
    bool cali_enable;
} Voltage_Measure_t;

extern Voltage_Measure_t mVoltage;

bool vol_mes_init();

uint16_t voltage_measure_init(Voltage_Measure_t *voltage_ptr);

void voltage_measure_deinit(Voltage_Measure_t *voltage_ptr);

float get_voltage(Voltage_Measure_t *voltage_ptr);

#endif  // VOLTAGE_MEASURE_HH

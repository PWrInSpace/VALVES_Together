#include "voltage_measure.h"
#include "esp_log.h"
#include <stdint.h>

/**************************  PRIVATE VARIABLES  *******************************/
static const char *TAG = "VOL_MEA";

adc_oneshot_unit_handle_t adc1_handle = NULL;
adc_cali_handle_t adc1_cali_handle = NULL;
bool adc1_cali_enabled = false;

Voltage_Measure_t mVoltage;


/**************************  PUBLIC FUNCTIONS  *******************************/
bool vol_mes_init(void) {

    //-------------ADC1 Calibration Init---------------//
    adc_cali_curve_fitting_config_t cali_config = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle) == ESP_OK) {
        adc1_cali_enabled = true;
        ESP_LOGI(TAG, "Calibration scheme for ADC1: Curve Fitting");
    } else {
        adc1_cali_enabled = false;
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration for ADC1");
    }

    return adc1_cali_enabled;
}

uint32_t get_voltage(Voltage_Measure_t *voltage_ptr) {
    ESP_ERROR_CHECK(adc_oneshot_read(*(voltage_ptr->adc_handle),
                                     voltage_ptr->adc_channel,
                                     (int *)&voltage_ptr->adc_raw));

    ESP_LOGI(TAG, "Raw ADC: %d", voltage_ptr->adc_raw);

    if (voltage_ptr->cali_enable) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(voltage_ptr->adc_cali_handle),
                                                voltage_ptr->adc_raw,
                                                (int *)&voltage_ptr->voltage));
        ESP_LOGI(TAG, "Calibrated Voltage: %d mV", voltage_ptr->voltage);
    }

    uint32_t bat_voltage = voltage_ptr->voltage * DIV_MULTIPLIER_VOL;
    return bat_voltage;
}

#include "voltage_measure.h"
#include "esp_log.h"
#include <stdint.h>
#include "adc_manager.h"

static const char *TAG = "VOL_MEA";

adc_oneshot_unit_handle_t adc1_handle = NULL;
adc_cali_handle_t adc1_cali_handle = NULL;
bool adc1_cali_enabled = false;

Voltage_Measure_t mVoltage;

bool vol_mes_init(void)
{
    mVoltage.adc_channel = 4U; 
    mVoltage.adc_handle  = &adc_manager.adc_handle;
    mVoltage.adc_cali_handle = &adc_manager.cali_handle;
    mVoltage.cali_enable = adc_manager.cali_enabled;

    adc_oneshot_chan_cfg_t config_vol = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_12,
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_manager.adc_handle,
                                               mVoltage.adc_channel,
                                               &config_vol));

    return true;
}
    


float get_voltage(Voltage_Measure_t *voltage_ptr) {
    if (voltage_ptr == NULL) {
        ESP_LOGE(TAG, "Voltage pointer is NULL!");
        return 0;
    }

    if (voltage_ptr->adc_handle == NULL) {
        ESP_LOGE(TAG, "ADC handle is NULL!");
        return 0;
    }

    if (voltage_ptr->cali_enable && voltage_ptr->adc_cali_handle == NULL) {
        ESP_LOGE(TAG, "ADC calibration handle is NULL!");
        return 0;
    }

    int raw = 0;
    esp_err_t err = adc_oneshot_read(*(voltage_ptr->adc_handle),
    voltage_ptr->adc_channel,
        (int *)&raw);
    if (err != ESP_OK) {
    ESP_LOGE(TAG, "ADC read failed! err=%d", err);
    return 0;
    }

    int voltage = raw;
    if (voltage_ptr->cali_enable) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(voltage_ptr->adc_cali_handle),
                                                raw,
                                                &voltage));
    }

    float bat_voltage = voltage * DIV_MULTIPLIER_VOL;
    voltage_ptr->adc_raw = raw;
    voltage_ptr->voltage = (uint32_t)bat_voltage;

    return bat_voltage/1000.0;
}
    
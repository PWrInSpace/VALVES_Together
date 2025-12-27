#include "adc_manager.h"
#include "esp_log.h"

static const char *TAG = "ADC_MANAGER";

ADC_Manager_t adc_manager = {
    .adc_handle = NULL,
    .cali_handle = NULL,
    .cali_enabled = false
};

bool adc_manager_init(void)
{
    ESP_LOGI(TAG, "Initializing ADC Manager...");

    // ADC1 init
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_manager.adc_handle));

    // Calibration
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
    };

    if (adc_cali_create_scheme_curve_fitting(&cali_config, &adc_manager.cali_handle) == ESP_OK) {
        adc_manager.cali_enabled = true;
        ESP_LOGI(TAG, "ADC Calibration ENABLED");
    } else {
        adc_manager.cali_enabled = false;
        ESP_LOGW(TAG, "Calibration NOT available!");
    }

    return true;
}

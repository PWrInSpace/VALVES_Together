#include "voltage_measure.h"
#include "esp_log.h"
#include <stdint.h>
#include "adc_manager.h"

static const char *TAG = "VOL_MEA";

// Maksymalna liczba kanałów do pomiaru napięcia
#define VOLTAGE_CHANNELS 3

typedef struct {
    uint8_t adc_channel;
    adc_oneshot_unit_handle_t *adc_handle;
    adc_cali_handle_t *adc_cali_handle;
    bool cali_enable;
    int adc_raw;
    uint32_t voltage;
} VoltageInput_t;

VoltageInput_t voltage_inputs[VOLTAGE_CHANNELS] = {
    { .adc_channel = 4 },
    { .adc_channel = 5 },
    { .adc_channel = 6 },
};


bool vol_mes_init(void)
{
    for (int i = 0; i < VOLTAGE_CHANNELS; i++) {
        voltage_inputs[i].adc_handle      = &adc_manager.adc_handle;
        voltage_inputs[i].adc_cali_handle = &adc_manager.cali_handle;
        voltage_inputs[i].cali_enable    = adc_manager.cali_enabled;

        adc_oneshot_chan_cfg_t cfg = {
            .bitwidth = ADC_BITWIDTH_12,
            .atten    = ADC_ATTEN_DB_12,
        };
        ESP_ERROR_CHECK(adc_oneshot_config_channel(*(voltage_inputs[i].adc_handle),
                                                   voltage_inputs[i].adc_channel,
                                                   &cfg));
    }

    ESP_LOGI(TAG, "Voltage measurement initialized for %d channels", VOLTAGE_CHANNELS);
    return true;
}

float get_voltage(uint8_t channel_index)
{
    if (channel_index >= VOLTAGE_CHANNELS) {
        ESP_LOGE(TAG, "Channel index out of range!");
        return 0;
    }

    VoltageInput_t *v = &voltage_inputs[channel_index];

    int raw = 0;
    esp_err_t err = adc_oneshot_read(*(v->adc_handle), v->adc_channel, &raw);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ADC read failed! err=%d", err);
        return 0;
    }
    #if defined(THERMISTOR_CALIBRATION) && (channel_index == 1 || channel_index == 2)
        int vol = 0;
        int sum = 0;
        for (int i = 0; i < 20; i++) {
            ESP_ERROR_CHECK(adc_oneshot_read(*(v->adc_handle), v->adc_channel, &raw));
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(v->adc_cali_handle), raw, &vol));
            sum += vol;
            vTaskDelay(pdMS_TO_TICKS(25));
        }
        sum /= 20;
        ESP_LOGI(TAG, "Calibrated voltage on channel %d: %d mV", channel_index, sum);
        return 8888.88; // Placeholder value
    #endif
    int voltage = raw;


    if (v->cali_enable) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(v->adc_cali_handle), raw, &voltage));
    }
    if(channel_index == 0) { // Battery voltage
        float bat_voltage = voltage * DIV_MULTIPLIER_VOL;
        v->adc_raw = raw;
        v->voltage = (uint32_t)bat_voltage;

        return bat_voltage / 1000.0;
    }
    else if (channel_index == 1) { // Thermistor1 voltage
        float vol1 = voltage * 1.3;
        v->adc_raw = raw;
        v->voltage = (uint32_t)vol1;

        float temp_c = vol1 * 1.0 + 0.0;

        return temp_c;
    }
    else if (channel_index == 2) {// Thermistor2 voltage
        float vol2 = voltage * 1.58;
        v->adc_raw = raw;
        v->voltage = (uint32_t)vol2;

        float temp_c = vol2 * 1.0 + 0.0;

        return temp_c;
    }
    return 9999.99; // Invalid channel index
}

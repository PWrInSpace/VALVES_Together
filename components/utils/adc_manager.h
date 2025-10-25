#pragma once
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include <stdbool.h>

typedef struct {
    adc_oneshot_unit_handle_t adc_handle;
    adc_cali_handle_t cali_handle;
    bool cali_enabled;
} ADC_Manager_t;

extern ADC_Manager_t adc_manager;

bool adc_manager_init(void);

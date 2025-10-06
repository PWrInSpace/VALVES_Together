 #include "pressure_sensor.h"

 /**************************  PRIVATE INCLUDES  ********************************/
 #include "esp_log.h"
 #include <stdint.h>
 
 /**************************  PRIVATE VARIABLES  *******************************/
 static const char *TAG = "PRESSURE_SENSOR";
 
 /**************************  PRIVATE FUNCTIONS  *******************************/
 uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {
     return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
 }
 
 /**************************  CODE *********************************************/
 Pressure_Manager_t pressure_manager;
 
 bool pressure_sensor_init(Pressure_Sensor_t *sensor_ptr) {
     //-------------ADC Channel Config---------------//
     adc_oneshot_chan_cfg_t config = {
         .bitwidth = ADC_BITWIDTH_DEFAULT,
         .atten = ADC_ATTEN_DB_12,   // zamiast DB_11
     };
     ESP_ERROR_CHECK(adc_oneshot_config_channel(*(sensor_ptr->adc_handle),
                                                sensor_ptr->adc_channel, &config));
     return true;
 }
 
 bool pressure_sensors_init()
 {
     pressure_manager.Initialized = 0;
 
     //-------------ADC1 Config---------------//
     adc_oneshot_unit_init_cfg_t init_config1 = {
         .unit_id = ADC_UNIT_1,
         .ulp_mode = ADC_ULP_MODE_DISABLE,
     };
     ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &pressure_manager.mADC_1));
 
     //-------------ADC1 Calibration Init---------------//
//-------------ADC1 Calibration Init---------------//
adc_cali_curve_fitting_config_t cali_config1 = {
    .atten    = ADC_ATTEN_DB_12,
    .bitwidth = ADC_BITWIDTH_DEFAULT,
};

if (adc_cali_create_scheme_curve_fitting(&cali_config1, &pressure_manager.mADC_1_cali) == ESP_OK) {
    pressure_manager.mADC_1_cali_enabled = true;
    ESP_LOGI(TAG, "Calibration scheme for ADC1: Curve Fitting");
} else {
    pressure_manager.mADC_1_cali_enabled = false;
    ESP_LOGW(TAG, "eFuse not burnt, skip software calibration for ADC1");
}

 
     //-------------Pressure Sensor 1 Init---------------//
     pressure_manager.mPressure1 = (Pressure_Sensor_t)PRESSURE_SENSOR_INIT(
         PRESSURE1_ADC_CHANNEL, &pressure_manager.mADC_1, &pressure_manager.mADC_1_cali,
         pressure_manager.mADC_1_cali_enabled);
     pressure_sensor_init(&pressure_manager.mPressure1);
 
     //-------------Pressure Sensor 2 Init---------------//
     pressure_manager.mPressure2 = (Pressure_Sensor_t)PRESSURE_SENSOR_INIT(
         PRESSURE2_ADC_CHANNEL, &pressure_manager.mADC_1, &pressure_manager.mADC_1_cali,
         pressure_manager.mADC_1_cali_enabled);
     pressure_sensor_init(&pressure_manager.mPressure2);
 
     pressure_manager.Initialized = 1;
     return true;
 }
 
 uint32_t get_pressure(Pressure_Sensor_t *sensor_ptr) {
     ESP_ERROR_CHECK(adc_oneshot_read(*(sensor_ptr->adc_handle),
                                      sensor_ptr->adc_channel,
                                      (int *)&sensor_ptr->adc_raw));
 
     ESP_LOGI(TAG, "PRESSURE_SENSOR Raw Data: %d", sensor_ptr->adc_raw);
 
     if (sensor_ptr->cali_enable) {
         ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(sensor_ptr->adc_cali_handle),
                                                 sensor_ptr->adc_raw,
                                                 (int *)&sensor_ptr->voltage));
         ESP_LOGI(TAG, "PRESSURE_SENSOR Voltage: %d mV", sensor_ptr->voltage);
     }
 
     // Prosta mapka surowego odczytu do 0–100%
     if(sensor_ptr->adc_raw < 430){
         return 0;
     } else if(sensor_ptr->adc_raw > 1850){
         return 100;
     } else {
         return map(sensor_ptr->adc_raw, 430, 1850, 0, 100);
     }
 }
 
 #include "pressure_sensor.h"

 /**************************  PRIVATE INCLUDES  ********************************/
 #include "esp_log.h"
 #include <stdint.h>
 #include "valve_board_config.h"
 
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
    /////////ESP32s3 version
//      pressure_manager.Initialized = 0;
 
//      //-------------ADC1 Config---------------//
//      adc_oneshot_unit_init_cfg_t init_config1 = {
//          .unit_id = ADC_UNIT_1,
//          .ulp_mode = ADC_ULP_MODE_DISABLE,
//      };
//      ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &pressure_manager.mADC_1));
 
//      //-------------ADC1 Calibration Init---------------//
// //-------------ADC1 Calibration Init---------------//
// adc_cali_curve_fitting_config_t cali_config1 = {
//     .atten    = ADC_ATTEN_DB_12,
//     .bitwidth = ADC_BITWIDTH_DEFAULT,
// };

// if (adc_cali_create_scheme_curve_fitting(&cali_config1, &pressure_manager.mADC_1_cali) == ESP_OK) {
//     pressure_manager.mADC_1_cali_enabled = true;
//     ESP_LOGI(TAG, "Calibration scheme for ADC1: Curve Fitting");
// } else {
//     pressure_manager.mADC_1_cali_enabled = false;
//     ESP_LOGW(TAG, "eFuse not burnt, skip software calibration for ADC1");
// }
    /////////ESP32s3 version

    pressure_manager.Initialized = 0;

    //-------------ADC1 Config---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &pressure_manager.mADC_1));

    // Konfiguracja kanału – dopasuj do swojego wejścia!
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(pressure_manager.mADC_1, ADC_CHANNEL_6, &config));

    //-------------ADC Calibration Init (ESP32)---------------//
    // ESP32 używa schematu „Line Fitting”
    adc_cali_line_fitting_config_t cali_config1 = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_11,
        .bitwidth = ADC_BITWIDTH_12,
    };

    if (adc_cali_create_scheme_line_fitting(&cali_config1, &pressure_manager.mADC_1_cali) == ESP_OK) {
        pressure_manager.mADC_1_cali_enabled = true;
        ESP_LOGI(TAG, "Calibration scheme for ADC1: Line Fitting");
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
 
 float get_pressure(Pressure_Sensor_t *sensor_ptr) {
    #ifdef PRESSURE_CALIBRATION
    double adc_sum = 0;
    const int samples = 100;
    for(int i = 0; i < samples; i++) {
        ESP_ERROR_CHECK(adc_oneshot_read(*(sensor_ptr->adc_handle),
                                         sensor_ptr->adc_channel,
                                         (int *)&sensor_ptr->adc_raw));
        adc_sum += sensor_ptr->adc_raw;
        vTaskDelay(pdMS_TO_TICKS(25)); // small delay between samples
    }
    ESP_LOGI(TAG, "PRESSURE_SENSOR Calibrated Raw Data: %.2f", adc_sum / (double)samples);
    return 99999.99; // return dummy pressure value during calibration
    #endif

     ESP_ERROR_CHECK(adc_oneshot_read(*(sensor_ptr->adc_handle),
                                      sensor_ptr->adc_channel,
                                      (int *)&sensor_ptr->adc_raw));
 
    //  ESP_LOGI(TAG, "PRESSURE_SENSOR Raw Data: %d", sensor_ptr->adc_raw);
 
    //  if (sensor_ptr->cali_enable) {
    //      ESP_ERROR_CHECK(adc_cali_raw_to_voltage(*(sensor_ptr->adc_cali_handle),
    //                                              sensor_ptr->adc_raw,
    //                                              (int *)&sensor_ptr->voltage));
    //     //  ESP_LOGI(TAG, "PRESSURE_SENSOR Voltage: %d mV", sensor_ptr->voltage);
    //  }
     float pressure;
     float zero_offset = 0; // not zero exactly but this is przesuniecie xd
     float multiplier = 0;
     #ifdef SERVO_N20_CONFIG //channel 1
        zero_offset = 10.1053; //70bar
        multiplier = 0.0316;
        #elif defined(SERVO_ETH_N2_CONFIG)
        zero_offset = 37.05;; // 300bar
        multiplier = 0.1523;
            #elif defined(SOL_ETH_CONFIG)  //240 raw zero 1375 na 50 bar (70bar sens)
        zero_offset = 7.7; //70bar
        multiplier = 0.028;
        #elif defined(SOL_N2O_N2_CONFIG)
        zero_offset = 7.05; //70bar
        multiplier = 0.042;
        #endif

    pressure = ((sensor_ptr->adc_raw * multiplier) - zero_offset);
    // ESP_LOGI(TAG, "PRESSURE_SENSOR Pressure: %.2f kPa", pressure);

     return pressure;
 }
 
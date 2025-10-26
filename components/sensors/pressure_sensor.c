 #include "pressure_sensor.h"

 /**************************  PRIVATE INCLUDES  ********************************/
 #include "esp_log.h"
 #include <stdint.h>
 #include "valve_board_config.h"
 #include "adc_manager.h"
 
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

     sensor_ptr->adc_handle = &adc_manager.adc_handle;
     sensor_ptr->adc_cali_handle = &adc_manager.cali_handle;
     sensor_ptr->cali_enable = adc_manager.cali_enabled;

     ESP_ERROR_CHECK(adc_oneshot_config_channel(*(sensor_ptr->adc_handle),
                                                sensor_ptr->adc_channel, &config));
     return true;
 }
 
 bool pressure_sensors_init()
 {
     pressure_manager.Initialized = 0;
 
     // Pressure 1
     pressure_manager.mPressure1 = (Pressure_Sensor_t)PRESSURE_SENSOR_INIT(
         PRESSURE1_ADC_CHANNEL,
         &adc_manager.adc_handle,
         &adc_manager.cali_handle,
         adc_manager.cali_enabled
     );
     pressure_sensor_init(&pressure_manager.mPressure1);
 
     // Pressure 2
     pressure_manager.mPressure2 = (Pressure_Sensor_t)PRESSURE_SENSOR_INIT(
         PRESSURE2_ADC_CHANNEL,
         &adc_manager.adc_handle,
         &adc_manager.cali_handle,
         adc_manager.cali_enabled
     );
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
    ESP_LOGI(TAG, "PRESSURE_SENSOR Calibrated Raw Data: %.5f", adc_sum / (double)samples);
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
    // ESP_LOGI(TAG, "PRESSURE_SENSOR Pressure: %.5f kPa", pressure);

     return pressure;
 }
 
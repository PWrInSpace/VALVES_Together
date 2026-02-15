#include "servo_config.h"
#include "esp_log.h"

#
#define TAG "servo_config.c"

#ifdef SERVO_N20_CONFIG
Servo_t servos[SERVO_COUNT] = {
    [N2O_FILL_SERVO] = SERVO_INIT(15),
};
#elif defined(SOL_N20_SERVO_ETH_CONFIG)
Servo_t servos[SERVO_COUNT] = {
    [ETH_FILL_SERVO] = SERVO_INIT(16),
};
#elif defined(SERVO_N2_CONFIG)
Servo_t servos[SERVO_COUNT] = {
    [N2_FILL_SERVO] = SERVO_INIT(15)
};
#else
Servo_t servos[SERVO_COUNT] = {};
#endif

esp_err_t init_multiple_servos()
{
  esp_err_t ret = ESP_OK;
  for (int i = 0; i < SERVO_COUNT; i++)
  {
    if (servo_init((ServoId_t)i) != EXIT_SUCCESS)
    {
      ESP_LOGE(TAG, "Failed to initialize servo %d", i);
      ret = ESP_LOG_ERROR;
    }
  }
  return ret;
}
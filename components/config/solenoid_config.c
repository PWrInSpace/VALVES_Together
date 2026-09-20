#include "solenoid_config.h"
#include "BoardData.h"
#include "Solenoid.h"
#include "esp_log.h"
#include "esp_timer.h"

#define TAG_IDLE "SOLENOID_IDLE"

#define IDLE_POWER_OFF_DELAY_MS 5000
#define IDLE_OPEN_MAX_OBC_STATE 1

#ifdef SOL_N2_CONFIG

Valve valves[NUM_OF_SOLENOIDS] = {
    [N2_FILL_SOL] = {.name = N2_FILL_SOL, .type = VALVE_NO},

};

#elif defined(SOL_ETH_SERVO_N2_CONFIG)

Valve valves[NUM_OF_SOLENOIDS] = {
    [ETH_FILL_SOL] = {.name = ETH_FILL_SOL, .type = VALVE_NO},

};

#elif defined(SOL_N20_SERVO_ETH_CONFIG)

Valve valves[NUM_OF_SOLENOIDS] = {
    [N20_FILL_SOL] = {.name = N20_FILL_SOL, .type = VALVE_NO},
};

#else

Valve valves[NUM_OF_SOLENOIDS] = {};

#endif

esp_err_t valves_init() {
  esp_err_t ret = 0;
  for (int i = 0; i < NUM_OF_SOLENOIDS; i++) {
    ret |= valve_init(&valves[i]);
  }
  return ret;
}

static void idle_power_off_timer_cb(void *arg) {
  (void)arg;
  uint8_t obc_state;
  get_obcState(&obc_state, portMAX_DELAY);

  if (obc_state > IDLE_OPEN_MAX_OBC_STATE) {
    ESP_LOGI(TAG_IDLE,
             "Idle timeout reached but obcState=%d, keeping solenoids closed",
             obc_state);
    return;
  }

  ESP_LOGI(
      TAG_IDLE,
      "Idle timeout reached (obcState=%d), turning off power to solenoids",
      obc_state);

  for (int i = 0; i < NUM_OF_SOLENOIDS; i++) {
    esp_err_t err = power_off_solenoid(valves[i].name);
    if (err != ESP_OK) {
      ESP_LOGE(TAG_IDLE, "Failed to power off solenoid %d: %s", 
        valves[i].name, esp_err_to_name(err));
    }
  }
}

esp_err_t schedule_idle_solenoid_power_off(void) {
  if (NUM_OF_SOLENOIDS == 0)
    return ESP_OK;

  const esp_timer_create_args_t timer_args = {.callback = idle_power_off_timer_cb,
                                              .name = "idle_sol_off"};

  esp_timer_handle_t timer_handle;
  esp_err_t err = esp_timer_create(&timer_args, &timer_handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG_IDLE, "Failed to create idle-open timer: %s",
             esp_err_to_name(err));
    return err;
  }

  return esp_timer_start_once(timer_handle, IDLE_POWER_OFF_DELAY_MS * 1000ULL);
}

#include "RGB_led_driver.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "RGB_LED";

#define TIMER LEDC_TIMER_1
#define MODE LEDC_LOW_SPEED_MODE

#define CHANNEL_R LEDC_CHANNEL_0
#define CHANNEL_G LEDC_CHANNEL_1
#define CHANNEL_B LEDC_CHANNEL_2

esp_err_t rgb_led_init(void) {
  esp_err_t err;

  ledc_timer_config_t ledc_timer = {.speed_mode = MODE,
                                    .duty_resolution = LEDC_TIMER_8_BIT,
                                    .timer_num = TIMER,
                                    .freq_hz = 5000,
                                    .clk_cfg = LEDC_AUTO_CLK};

  err = ledc_timer_config(&ledc_timer);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Timer config failed");
    return err;
  }

  ledc_channel_config_t ledc_channel[3] = {{.gpio_num = LED_R_GPIO,
                                            .speed_mode = MODE,
                                            .channel = CHANNEL_R,
                                            .intr_type = LEDC_INTR_DISABLE,
                                            .timer_sel = TIMER,
                                            .duty = 0,
                                            .hpoint = 0},
                                           {.gpio_num = LED_G_GPIO,
                                            .speed_mode = MODE,
                                            .channel = CHANNEL_G,
                                            .intr_type = LEDC_INTR_DISABLE,
                                            .timer_sel = TIMER,
                                            .duty = 0,
                                            .hpoint = 0},
                                           {.gpio_num = LED_B_GPIO,
                                            .speed_mode = MODE,
                                            .channel = CHANNEL_B,
                                            .intr_type = LEDC_INTR_DISABLE,
                                            .timer_sel = TIMER,
                                            .duty = 0,
                                            .hpoint = 0}};

  for (int i = 0; i < 3; i++) {
    err = ledc_channel_config(&ledc_channel[i]);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Channel config failed for pin %d",
               ledc_channel[i].gpio_num);
      return err;
    }
  }

  ESP_LOGI(TAG, "RGB LED initialized successfully");
  return ESP_OK;
}

esp_err_t rgb_led_set_channels(uint8_t channel_r, uint8_t channel_g,
                               uint8_t channel_b) {
  esp_err_t err;

  err = ledc_set_duty(MODE, CHANNEL_R, (255 - channel_r));
  if (err != ESP_OK)
    return err;
  err = ledc_update_duty(MODE, CHANNEL_R);
  if (err != ESP_OK)
    return err;

  err = ledc_set_duty(MODE, CHANNEL_G, (255 - channel_g));
  if (err != ESP_OK)
    return err;
  err = ledc_update_duty(MODE, CHANNEL_G);
  if (err != ESP_OK)
    return err;

  err = ledc_set_duty(MODE, CHANNEL_B, (255 - channel_b));
  if (err != ESP_OK)
    return err;
  err = ledc_update_duty(MODE, CHANNEL_B);
  if (err != ESP_OK)
    return err;

  return ESP_OK;
}

esp_err_t rgb_led_set_hex_color(led_color_t hex_color) {
  uint8_t r = (hex_color >> 16) & 0xFF;
  uint8_t g = (hex_color >> 8) & 0xFF;
  uint8_t b = hex_color & 0xFF;

  return rgb_led_set_channels(r, g, b);
}

esp_err_t rgb_turn_off(void) { return rgb_led_set_channels(0, 0, 0); }
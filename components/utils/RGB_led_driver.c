#include "RGB_led_driver.h"
#include "esp_log.h"
#include "led_strip.h"

static const char *TAG = "ARGB_LED";
static led_strip_handle_t s_led_strip = NULL;

esp_err_t rgb_led_init(void) {
  led_strip_config_t strip_config = {
    .strip_gpio_num = ARGB_DIN_GPIO,
    .max_leds = 1, // only one ARGB led on pcb
    .led_pixel_format = LED_PIXEL_FORMAT_GRB,
    .led_model = LED_MODEL_WS2812,
    .flags.invert_out = false,
  };

  led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 1e7, // 10 MHz
    .mem_block_symbols = 64,
    .flags.with_dma = false,
  };

  esp_err_t err = led_strip_new_rmt_device(&strip_config, &rmt_config, &s_led_strip);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "init failed: %s", esp_err_to_name(err));
    return err;
  }

  err = led_strip_clear(s_led_strip);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "clear failed: %s", esp_err_to_name(err));
    return err;
  }

  ESP_LOGI(TAG, "initialized on GPIO %d", ARGB_DIN_GPIO);
  return ESP_OK;
}

esp_err_t rgb_led_set_channels(uint8_t channel_r, uint8_t channel_g, uint8_t channel_b) {
  if (s_led_strip == NULL) return ESP_ERR_INVALID_STATE;

  esp_err_t err = led_strip_set_pixel(s_led_strip, 0, channel_r, channel_g, channel_b); // only one ARGB led on pcb
  if (err != ESP_OK) return err;

  return led_strip_refresh(s_led_strip);
}

esp_err_t rgb_led_set_color(led_color_t color) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;

  return rgb_led_set_channels(r, g, b);
}

esp_err_t rgb_turn_off(void) {
  if (s_led_strip == NULL) return ESP_ERR_INVALID_STATE;
  return led_strip_clear(s_led_strip);
}

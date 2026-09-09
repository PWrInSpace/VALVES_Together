#ifndef rgb_led_h
#define rgb_led_h

#include "esp_err.h"
#include <stdint.h>

#define ARGB_DIN_GPIO 11

typedef enum {
  RGB_WHITE = 0xFFFFFF,
  RGB_RED = 0xFF0000,
  RGB_GREEN = 0x00FF00,
  RGB_BLUE = 0x0000FF,
  RGB_YELLOW = 0xFF8800,
  RGB_CYAN = 0x00FFFF,
  RGB_MAGENTA = 0xFF0088,
  RGB_PURPLE = 0x8800FF,
  RGB_ORANGE = 0xFF4400
} led_color_t;

esp_err_t rgb_led_init(void);

esp_err_t rgb_led_set_channels(uint8_t channel_r, uint8_t channel_g, uint8_t channel_b);

esp_err_t rgb_led_set_color(led_color_t color);

esp_err_t rgb_turn_off(void);

#endif // rgb_led_h

#ifndef rgb_led_h
#define rgb_led_h

#include <stdint.h>
#include "esp_err.h"

#define LED_R_GPIO 39
#define LED_G_GPIO 40
#define LED_B_GPIO 41

typedef enum {
    RGB_WHITE = 0xFFFFFF,
    RGB_RED = 0xFF0000,
    RGB_GREEN = 0x00FF00,
    RGB_BLUE = 0x0000FF,
    RGB_YELLOW = 0xFFFF00,
    RGB_CYAN = 0x00FFFF,
    RGB_MAGENTA = 0xFF00FF,
    RGB_PURPLE = 0xAA00FF,
    RGB_ORANGE = 0xFFA500
} led_color_t;

esp_err_t rgb_led_init(void);

esp_err_t rgb_led_set_channels(uint8_t channel_r, uint8_t channel_g, uint8_t channel_b);

esp_err_t rgb_led_set_color(led_color_t color);

esp_err_t rgb_turn_off(void);

#endif
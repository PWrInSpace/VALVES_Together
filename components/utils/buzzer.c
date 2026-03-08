#include "buzzer.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

esp_err_t buzzer_init() {
  ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                    .timer_num = LEDC_TIMER_0,
                                    .duty_resolution = LEDC_TIMER_10_BIT,
                                    .freq_hz = 1000,
                                    .clk_cfg = LEDC_AUTO_CLK};
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel = {.gpio_num = BUZZER_GPIO,
                                        .speed_mode = LEDC_LOW_SPEED_MODE,
                                        .channel = LEDC_CHANNEL_0,
                                        .intr_type = LEDC_INTR_DISABLE,
                                        .timer_sel = LEDC_TIMER_0,
                                        .duty = 0,
                                        .hpoint = 0};
  ledc_channel_config(&ledc_channel);

  ESP_LOGI("BUZZER", "Buzzer initialized on GPIO %d", BUZZER_GPIO);
  return ESP_OK;
}

void play_tone(int freq, int duration_ms) {
  int duty = (1 << 10) / 2; // 50% duty cycle
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(duration_ms));
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(50));
}

void imperial_march() {
  // Ta-dam Ta-dam Ta-dam Ta-daaaaam
  play_tone(NOTE_A4, 500);
  play_tone(NOTE_A4, 500);
  play_tone(NOTE_A4, 500);
  play_tone(NOTE_F4, 350);
  play_tone(NOTE_C5, 150);

  play_tone(NOTE_A4, 500);
  play_tone(NOTE_F4, 350);
  play_tone(NOTE_C5, 150);
  play_tone(NOTE_A4, 1000);

  // Druga część
  play_tone(NOTE_E5, 500);
  play_tone(NOTE_E5, 500);
  play_tone(NOTE_E5, 500);
  play_tone(NOTE_F5, 350);
  play_tone(NOTE_C5, 150);

  play_tone(NOTE_AS4, 500);
  play_tone(NOTE_F4, 350);
  play_tone(NOTE_C5, 150);
  play_tone(NOTE_A4, 1000);
}

void ode_to_joy() {
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_F4, 300);
  play_tone(NOTE_G4, 300);

  play_tone(NOTE_G4, 300);
  play_tone(NOTE_F4, 300);
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_D4, 300);

  play_tone(NOTE_C4, 300);
  play_tone(NOTE_C4, 300);
  play_tone(NOTE_D4, 300);
  play_tone(NOTE_E4, 300);

  play_tone(NOTE_E4, 450);
  play_tone(NOTE_D4, 150);
  play_tone(NOTE_D4, 600);
}
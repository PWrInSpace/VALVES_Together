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
  if (freq <= 0) {
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    return;
  }
  int duty = (1 << 10) / 2; // 50% duty cycle
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, freq);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(duration_ms));
  ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(20));
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

void harry_potter_theme() {
  play_tone(NOTE_B4, 300);
  play_tone(NOTE_E5, 600);
  play_tone(NOTE_G5, 300);
  play_tone(NOTE_FS5, 300);
  play_tone(NOTE_E5, 600);

  play_tone(NOTE_B5, 300);
  play_tone(NOTE_A5, 600);
  play_tone(NOTE_FS5, 600);

  play_tone(NOTE_E5, 300);
  play_tone(NOTE_G5, 600);
  play_tone(NOTE_FS5, 300);
  play_tone(NOTE_DS5, 300);
  play_tone(NOTE_F5, 600);

  play_tone(NOTE_B4, 300);
  play_tone(NOTE_E5, 600);
}

void good_bad_ugly() {
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 300);
  play_tone(NOTE_E4, 300);

  play_tone(NOTE_A4, 600);
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 300);

  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 800);
}

void mario_theme() {
  play_tone(NOTE_E5, 150);
  play_tone(NOTE_E5, 150);
  play_tone(0, 150);

  play_tone(NOTE_E5, 150);
  play_tone(0, 150);
  play_tone(NOTE_C5, 150);
  play_tone(NOTE_E5, 150);

  play_tone(NOTE_G5, 400);
  play_tone(0, 400);

  play_tone(NOTE_G4, 400);
}

void nokia_tune() {
  play_tone(NOTE_E5, 250);
  play_tone(NOTE_D5, 250);
  play_tone(NOTE_FS4, 500);
  play_tone(NOTE_GS4, 500);

  play_tone(NOTE_CS5, 250);
  play_tone(NOTE_B4, 250);
  play_tone(NOTE_D4, 500);
  play_tone(NOTE_E4, 500);
}

void beep_single() { play_tone(1000, 800); }

void beep_double() {
  play_tone(1000, 300);
  vTaskDelay(pdMS_TO_TICKS(150));
  play_tone(1000, 300);
}

void beep_triple() {
  play_tone(1000, 200);
  vTaskDelay(pdMS_TO_TICKS(100));
  play_tone(1000, 200);
  vTaskDelay(pdMS_TO_TICKS(100));
  play_tone(1000, 200);
}

void beep_quatro() {
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
}

// ---------------------------------------------------------------------------
// Crab Rave - 3-part arrangement, BPM=128
// Q=469ms  E=234ms  S=117ms  H=938ms
// Key: D major  (D E F# G A B C#)
// ---------------------------------------------------------------------------
#define Q 469
#define E 234
#define S 117
#define H 938

// PART 1 - main melody (SERVO_N20_CONFIG)
void crab_rave_melody(void) {
  // Intro / main riff
  play_tone(NOTE_D5, E);
  play_tone(NOTE_D5, S);
  play_tone(NOTE_E5, S);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_A5, E);

  play_tone(NOTE_G5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_E5, E);
  play_tone(NOTE_D5, E);

  play_tone(NOTE_E5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_A5, E);
  play_tone(NOTE_B5, E);

  play_tone(NOTE_A5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_D5, H);

  // Second phrase
  play_tone(NOTE_E5, E);
  play_tone(NOTE_E5, S);
  play_tone(NOTE_FS5, S);
  play_tone(NOTE_G5, E);
  play_tone(NOTE_A5, E);

  play_tone(NOTE_B5, E);
  play_tone(NOTE_A5, E);
  play_tone(NOTE_G5, E);
  play_tone(NOTE_FS5, E);

  play_tone(NOTE_A5, E);
  play_tone(NOTE_G5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_E5, E);

  play_tone(NOTE_D5, H);
  play_tone(0, H);
}

// PART 2 - counter-melody / harmony (SOL_N20_SERVO_ETH_CONFIG)
void crab_rave_harmony(void) {
  // Harmony follows the chord tones a third below the melody
  play_tone(NOTE_B4, E);
  play_tone(NOTE_B4, S);
  play_tone(NOTE_CS5, S);
  play_tone(NOTE_D5, E);
  play_tone(NOTE_FS5, E);

  play_tone(NOTE_E5, E);
  play_tone(NOTE_D5, E);
  play_tone(NOTE_CS5, E);
  play_tone(NOTE_B4, E);

  play_tone(NOTE_CS5, E);
  play_tone(NOTE_D5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_G5, E);

  play_tone(NOTE_FS5, E);
  play_tone(NOTE_D5, E);
  play_tone(NOTE_B4, H);

  // Second phrase
  play_tone(NOTE_CS5, E);
  play_tone(NOTE_CS5, S);
  play_tone(NOTE_D5, S);
  play_tone(NOTE_E5, E);
  play_tone(NOTE_FS5, E);

  play_tone(NOTE_G5, E);
  play_tone(NOTE_FS5, E);
  play_tone(NOTE_E5, E);
  play_tone(NOTE_D5, E);

  play_tone(NOTE_FS5, E);
  play_tone(NOTE_E5, E);
  play_tone(NOTE_D5, E);
  play_tone(NOTE_CS5, E);

  play_tone(NOTE_B4, H);
  play_tone(0, H);
}

// PART 3 - bass line (SOL_ETH_CONFIG)
void crab_rave_bass(void) {
  // Root-fifth bass pattern on chord changes: D A Bm G
  // Bar 1-2  D major
  play_tone(NOTE_D4, Q);
  play_tone(NOTE_A4, Q);
  play_tone(NOTE_D4, Q);
  play_tone(NOTE_A4, Q);

  // Bar 3-4  A major
  play_tone(NOTE_A3, Q);
  play_tone(NOTE_E4, Q);
  play_tone(NOTE_A3, Q);
  play_tone(NOTE_E4, Q);

  // Bar 5-6  Bm
  play_tone(NOTE_B3, Q);
  play_tone(NOTE_FS4, Q);
  play_tone(NOTE_B3, Q);
  play_tone(NOTE_FS4, Q);

  // Bar 7-8  G major
  play_tone(NOTE_G4, Q);
  play_tone(NOTE_D4, Q);
  play_tone(NOTE_G4, Q);
  play_tone(NOTE_D4, Q);

  play_tone(0, H);
}

#undef Q
#undef E
#undef S
#undef H
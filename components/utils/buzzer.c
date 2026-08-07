#include "buzzer.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

// Octave 4
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494

// Octave 3
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247

// Octave 4 (extra)
#define NOTE_DS4 311

// Octave 5
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988

// Octave 6
#define NOTE_D6 1175
#define NOTE_E6 1319
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_A6 1760

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

static void play_tone(int freq, int duration_ms) {
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

static void imperial_march(void) {
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

static void ode_to_joy(void) {
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

static void harry_potter_theme(void) {
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

static void good_bad_ugly() {
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 300);
  play_tone(NOTE_E4, 300);

  play_tone(NOTE_A4, 600);
  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 300);

  play_tone(NOTE_E4, 300);
  play_tone(NOTE_A4, 800);
}

static void mario_theme() {
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

static void nokia_tune() {
  play_tone(NOTE_E5, 250);
  play_tone(NOTE_D5, 250);
  play_tone(NOTE_FS4, 500);
  play_tone(NOTE_GS4, 500);

  play_tone(NOTE_CS5, 250);
  play_tone(NOTE_B4, 250);
  play_tone(NOTE_D4, 500);
  play_tone(NOTE_E4, 500);
}

static void beep_single() { play_tone(1000, 800); }

static void beep_double() {
  play_tone(1000, 300);
  vTaskDelay(pdMS_TO_TICKS(150));
  play_tone(1000, 300);
}

static void beep_triple() {
  play_tone(1000, 200);
  vTaskDelay(pdMS_TO_TICKS(100));
  play_tone(1000, 200);
  vTaskDelay(pdMS_TO_TICKS(100));
  play_tone(1000, 200);
}

static void beep_quatro() {
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
  vTaskDelay(pdMS_TO_TICKS(80));
  play_tone(1000, 150);
}

static void charger_connected() {
  play_tone(NOTE_D5, 120);
  play_tone(NOTE_FS5, 120);
  play_tone(NOTE_A5, 120);
  play_tone(NOTE_D6, 300);
}

static void charger_disconnected() {
  play_tone(NOTE_G4, 200);
  play_tone(NOTE_C4, 450);
}

static void init_error() {
  for (int i = 0; i < 3; ++i) {
    play_tone(NOTE_AS3, 250);
    play_tone(NOTE_A3, 400);
    vTaskDelay(pdMS_TO_TICKS(120));
  }
}

static void low_battery() {
  play_tone(NOTE_B4, 200);
  play_tone(NOTE_AS4, 220);
  play_tone(NOTE_A4, 260);
  play_tone(NOTE_GS4, 500);
}

static void all_ok() {
  play_tone(NOTE_FS6, 100);
  play_tone(NOTE_A6, 200);
}

static const sound_fn_t sound_functions[SOUND_COUNT] = {
    [SOUND_ALL_OK] = all_ok,
    [SOUND_CHARGER_CONNECTED] = charger_connected,
    [SOUND_CHARGER_DISCONNECTED] = charger_disconnected,
    [SOUND_LOW_BATTERY] = low_battery,
    [SOUND_INIT_ERROR] = init_error,
    [SOUND_SINGLE_BEEP] = beep_single,
    [SOUND_DOUBLE_BEEP] = beep_double,
    [SOUND_TRIPLE_BEEP] = beep_triple,
    [SOUND_QUADRUPLE_BEEP] = beep_quatro,
    [SOUND_IMPERIAL_MARCH] = imperial_march,
    [SOUND_ODE_TO_JOY] = ode_to_joy,
    [SOUND_HARRY_POTTER_THEME] = harry_potter_theme,
    [SOUND_NONE] = NULL,
};

void play_sound(sound_id_t sound) {
  if (sound >= SOUND_COUNT || sound_functions[sound] == NULL || sound < 1) {
    return;
  }
  sound_functions[sound]();
}

#undef Q
#undef E
#undef S
#undef H
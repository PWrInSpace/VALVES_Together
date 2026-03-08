#ifndef buzzer_h
#define buzzer_h

#include "esp_err.h"

#define BUZZER_GPIO 36

#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_AS4 466
#define NOTE_F4  349
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_F5  698

esp_err_t buzzer_init();
void play_tone(int freq, int duration_ms);
void imperial_march();
void ode_to_joy();

#endif // buzzer_h
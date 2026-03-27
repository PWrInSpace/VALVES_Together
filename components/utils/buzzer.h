#ifndef buzzer_h
#define buzzer_h

#include "esp_err.h"

#define BUZZER_GPIO 36

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

// Octave 5
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_A5 880
#define NOTE_B5 988

esp_err_t buzzer_init();
void play_tone(int freq, int duration_ms);
void imperial_march();
void ode_to_joy();
void harry_potter_theme();
void nokia_tune();
void mario_theme();
void good_bad_ugly();
void beep_single();
void beep_double();
void beep_triple();
void beep_quatro();
#endif // buzzer_h
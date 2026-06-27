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

// Crab Rave - 3-part arrangement (one part per board config)
void crab_rave_melody(void);  // SERVO_N20_CONFIG        - main melody
void crab_rave_harmony(void); // SOL_N20_SERVO_ETH_CONFIG - counter-melody
void crab_rave_bass(void);    // SOL_ETH_CONFIG           - bass line

#endif // buzzer_h
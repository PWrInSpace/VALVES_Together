#ifndef buzzer_h
#define buzzer_h

#include "esp_err.h"

#define BUZZER_GPIO 42

esp_err_t buzzer_init();

typedef enum {
  SOUND_NONE = 0,
  SOUND_ALL_OK,
  SOUND_CHARGER_CONNECTED,
  SOUND_CHARGER_DISCONNECTED,
  SOUND_LOW_BATTERY,
  SOUND_INIT_ERROR,
  SOUND_SINGLE_BEEP,
  SOUND_DOUBLE_BEEP,
  SOUND_TRIPLE_BEEP,
  SOUND_QUADRUPLE_BEEP,
  SOUND_IMPERIAL_MARCH,
  SOUND_ODE_TO_JOY,
  SOUND_HARRY_POTTER_THEME,
  SOUND_COUNT
} sound_id_t;

typedef void (*sound_fn_t)(void);

void play_sound(sound_id_t sound);

#endif // buzzer_h
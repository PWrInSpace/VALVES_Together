#ifndef BUZZER_TASK_H
#define BUZZER_TASK_H

#include "buzzer.h"
#include <stdbool.h>

#include "freertos/FreeRTOS.h"

bool run_buzzer_task(void);
bool play_buzzer_sound(sound_id_t sound);

#endif /* BUZZER_TASK_H */

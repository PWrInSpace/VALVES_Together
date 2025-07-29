///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 06.05.2025 by Szymon Rzewuski
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains the definitions of functions for the main application task
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_APP_TASK_H
#define PWRINSPACE_APP_TASK_H

#include "esp_err.h"

void app_task(void *arg);
esp_err_t app_task_init(void);
esp_err_t app_task_deinit(void);

#endif //PWRINSPACE_APP_TASK_H
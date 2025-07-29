///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 27.01.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the system configuration including mcu config, can api config
/// and hardware config to be implemented by user
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_BOARD_CONFIG_H
#define PWRINSPACE_BOARD_CONFIG_H

#include "esp_err.h"

typedef struct {
    char board_name[32];
} board_config_t;

extern board_config_t config;

esp_err_t board_config_init(void);

#endif /* PWRINSPACE_BOARD_CONFIG_H */


// RTR -> TEN SAM ID U MASTER I SLAVE BO RTR PRZEGRYWA WALKE O MIEJSCE!
#include "pressure_driver.h"

// DATA(name, type, default_value)
// DATA_ARRAY(name, type, array_size, default_value)
// SECTION_BEGIN(name)
// SECTION_END(name)

// obecnie wspierane typy: int32_t, uint8_t, float, double, char, char[]
// możliwość rozszerzenia wspieranych typów w pliku flash.c (należy na samym dole dodać parser i zaktualizować funkcje update_field)

#define CONFIG_FIELDS                                                         \
    SECTION_BEGIN(press_calibr)                                               \
    DATA(sensor_0_volt_0, float, PRESSURE_DRIVER_DEFAULT_MIN_VOLTAGE)         \
    DATA(sensor_0_volt_1, float, PRESSURE_DRIVER_DEFAULT_MAX_VOLTAGE)         \
    DATA(sensor_0_press_1, float, PRESSURE_DRIVER_DEFAULT_MAX_PRESSURE)       \
    DATA(sensor_1_volt_0, float, PRESSURE_DRIVER_DEFAULT_MIN_VOLTAGE)         \
    DATA(sensor_1_volt_1, float, PRESSURE_DRIVER_DEFAULT_MAX_VOLTAGE)         \
    DATA(sensor_1_press_1, float, PRESSURE_DRIVER_DEFAULT_MAX_PRESSURE)       \
    DATA(sensor_2_volt_0, float, PRESSURE_DRIVER_DEFAULT_MIN_VOLTAGE)         \
    DATA(sensor_2_volt_1, float, PRESSURE_DRIVER_DEFAULT_MAX_VOLTAGE)         \
    DATA(sensor_2_press_1, float, PRESSURE_DRIVER_DEFAULT_MAX_PRESSURE)       \
    DATA(sensor_3_volt_0, float, PRESSURE_DRIVER_DEFAULT_MIN_VOLTAGE)         \
    DATA(sensor_3_volt_1, float, PRESSURE_DRIVER_DEFAULT_MAX_VOLTAGE)         \
    DATA(sensor_3_press_1, float, PRESSURE_DRIVER_DEFAULT_MAX_PRESSURE)       \
    SECTION_END(press_calibr)                                                 \
    SECTION_BEGIN(servo_calibr)                                               \
    DATA(open_pos, uint8_t, 0)                                                \
    DATA(close_pos, uint8_t, 101)                                             \
    SECTION_END(servo_calibr)
// jeżeli ktokolwiek usunie tą linie to kompilator zacznie drzeć ryja, chyba że dodasz pustą linię po ostatniej definicji :)
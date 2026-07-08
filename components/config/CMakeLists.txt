idf_component_register(  SRC_DIRS "."
                         INCLUDE_DIRS "."
                         REQUIRES nvs driver cli mcu_config timers app solenoids servo esp_now sensors commands utils sd_card debug charger)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")


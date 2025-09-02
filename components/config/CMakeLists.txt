idf_component_register(  SRC_DIRS "."
                         INCLUDE_DIRS "."
                         REQUIRES driver cli mcu_config timers app solenoids servo esp_now)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")


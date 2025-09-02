idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi nvs_flash driver config commands Board_Data solenoids)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")

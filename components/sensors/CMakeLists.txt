idf_component_register(  SRC_DIRS "."
                         INCLUDE_DIRS "."
                         REQUIRES driver esp_adc soc config utils nvs)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")
idf_component_register(  SRC_DIRS "."
                         INCLUDE_DIRS "."
                         REQUIRES config timers utils esp_now sd_card)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format" "-Wall" "-Werror")

set(main "${CMAKE_CURRENT_SOURCE_DIR}/main.c" "${CMAKE_CURRENT_SOURCE_DIR}/panel_bluetooth.c" "${CMAKE_CURRENT_SOURCE_DIR}/panel_video.c" "${CMAKE_CURRENT_SOURCE_DIR}/panel_movie.c")

idf_component_register(SRCS ${main}
                        INCLUDE_DIRS "include" 
                        REQUIRES bsp_extra bsp_display bsp_illuminate bsp_i2c esp_timer bsp_stc8h1kxx bsp_audio bsp_mic json esp_hid bt nvs_flash fatfs sdmmc esp_driver_jpeg mbedtls)
                                 



target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/day.rgb565" BINARY)
target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/night.rgb565" BINARY)

target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/branch.argb" BINARY)
target_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/setup_qr.rgb565" BINARY)

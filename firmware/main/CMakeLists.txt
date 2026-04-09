idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES logic esp32 include driver nvs_flash esp_driver_uart vfs)

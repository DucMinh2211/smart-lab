idf_component_register(SRCS "DHT22.cpp" "ESP32Hardware.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES include esp_driver_gpio esp_timer)

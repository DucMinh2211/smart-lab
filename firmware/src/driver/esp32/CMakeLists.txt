idf_component_register(SRCS "DHT22.cpp" "DHT20.cpp" "ESP32Hardware.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES include esp_driver_gpio esp_timer led_strip esp_driver_i2c esp_driver_ledc)

#include "ESP32Hardware.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h" 
#include "led_strip_types.h"

static const char* TAG = "ESP32Hardware";

ESP32Hardware::ESP32Hardware() {
    // 1. Setup I2C Bus for DHT20/AHT20
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = I2C_NUM_0;
    bus_cfg.sda_io_num = I2C_SDA_PIN;
    bus_cfg.scl_io_num = I2C_SCL_PIN;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_cfg.glitch_ignore_cnt = 7;
    bus_cfg.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &i2c_bus));

    // 2. Dynamic Sensor Selection (Abstraction)
    DHT20* d20 = new DHT20(i2c_bus);
    if (d20->read()) {
        sensor = d20;
        ESP_LOGI(TAG, "Sensor: DHT20 (I2C) detected and using.");
    } else {
        delete d20;
        sensor = new DHT22(DHT22_PIN);
        ESP_LOGI(TAG, "Sensor: DHT20 not found, fallback to DHT22 (GPIO %d).", DHT22_PIN);
    }

    // 3. Setup GPIOs (Buzzer, PIR)
    gpio_config_t io_conf = {};
    
    // Buzzer
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << ALARM_BUZZER_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // PIR Sensor with Interrupt
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIR_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // 4. Setup NeoPixel Strip
    led_strip_config_t strip_conf = {};
    strip_conf.strip_gpio_num = RGB_LED_PIN;
    strip_conf.max_leds = RGB_LED_NUM;
    strip_conf.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
    strip_conf.led_model = LED_MODEL_WS2812;

    led_strip_rmt_config_t rmt_conf = {};
    rmt_conf.resolution_hz = 10 * 1000 * 1000;
    rmt_conf.clk_src = RMT_CLK_SRC_DEFAULT;

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_conf, &rmt_conf, &led_strip));
    led_strip_clear(led_strip);
    
    // Initial Test: Green
    for (int i = 0; i < RGB_LED_NUM; i++) led_strip_set_pixel(led_strip, i, 0, 100, 0);
    led_strip_refresh(led_strip);

    ESP_LOGI(TAG, "Hardware initialized. LED: %d, PIR: %d, Buzzer: %d", 
             RGB_LED_PIN, PIR_PIN, ALARM_BUZZER_PIN);
}

float ESP32Hardware::getTemperature() {
    if (sensor->read()) return sensor->getTemperature();
    return 0;
}

float ESP32Hardware::getHumidity() {
    return sensor->getHumidity();
}

void ESP32Hardware::setLedMode(LedMode mode) {
    uint32_t r = 0, g = 0, b = 0;
    switch (mode) {
        case LedMode::NORMAL:       g = 100; break;
        case LedMode::WARNING:      r = 100; g = 50; break;
        case LedMode::EMERGENCY:    r = 255; break;
        case LedMode::LIGHTING:     b = 100; break;
        case LedMode::OFF:          r = 0, g = 0, b = 0; break;
    }
    for (int i = 0; i < RGB_LED_NUM; i++) led_strip_set_pixel(led_strip, i, r, g, b);
    led_strip_refresh(led_strip);
}

void ESP32Hardware::setAlarm(bool active) {
    gpio_set_level((gpio_num_t)ALARM_BUZZER_PIN, active ? 1 : 0);
}

bool ESP32Hardware::isMotionDetected() {
    return gpio_get_level((gpio_num_t)PIR_PIN) == 1;
}

unsigned long ESP32Hardware::getMillis() {
    return (unsigned long)(esp_timer_get_time() / 1000);
}

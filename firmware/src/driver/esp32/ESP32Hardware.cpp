#include "ESP32Hardware.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h" 
#include "led_strip_types.h"

static const char* TAG = "ESP32Hardware";

// Yolo:Bit S3 AIoT Kit Pin Mapping
#define I2C_SDA_PIN         GPIO_NUM_20  // P20 for S3
#define I2C_SCL_PIN         GPIO_NUM_19  // P19 for S3
#define ALARM_BUZZER_PIN    GPIO_NUM_3   // P3
#define PIR_PIN             GPIO_NUM_2   // P2
#define DHT22_PIN           GPIO_NUM_1   // P1
#define RGB_LED_PIN         GPIO_NUM_0   // P0
#define RGB_LED_NUM         4

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

    // 2. Initialize Sensor instances
    dht20 = new DHT20(i2c_bus);
    dht22 = new DHT22(DHT22_PIN);

    // 3. Setup GPIOs (Buzzer, PIR)
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << ALARM_BUZZER_PIN);
    gpio_config(&io_conf);

    // Setup PIR Sensor with Interrupt
    io_conf.intr_type = GPIO_INTR_POSEDGE; // Trigger on rising edge (Motion detected)
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
    rmt_conf.resolution_hz = 10 * 1000 * 1000; // 10MHz

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_conf, &rmt_conf, &led_strip));
    led_strip_clear(led_strip);

    ESP_LOGI(TAG, "Hardware S3 initialized (SDA:P20, SCL:P19, Buzzer:P3, PIR:P2, LEDs:P0).");
}

float ESP32Hardware::getTemperature() {
    if (use_dht20) {
        if (dht20->read()) return dht20->getTemperature();
        ESP_LOGW(TAG, "DHT20 read failed, falling back to DHT22...");
    }
    dht22->read();
    return dht22->getTemperature();
}

float ESP32Hardware::getHumidity() {
    if (use_dht20) return dht20->getHumidity();
    return dht22->getHumidity();
}

void ESP32Hardware::setLedMode(LedMode mode) {
    uint32_t r = 0, g = 0, b = 0;
    switch (mode) {
        case LedMode::NORMAL:    g = 100; break; // Green
        case LedMode::WARNING:   r = 100; g = 50; break; // Orange
        case LedMode::EMERGENCY: r = 255; break; // Bright Red
        case LedMode::OFF:       break;
    }
    for (int i = 0; i < RGB_LED_NUM; i++) led_strip_set_pixel(led_strip, i, r, g, b);
    led_strip_refresh(led_strip);
}

void ESP32Hardware::setAlarm(bool active) {
    gpio_set_level(ALARM_BUZZER_PIN, active ? 1 : 0);
}

bool ESP32Hardware::isMotionDetected() {
    return gpio_get_level(PIR_PIN) == 1;
}

unsigned long ESP32Hardware::getMillis() {
    return (unsigned long)(esp_timer_get_time() / 1000);
}

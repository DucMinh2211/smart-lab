#include "ESP32Hardware.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char* TAG = "ESP32Hardware";

// Yolo:Bit Pins
#define ALARM_BUZZER_PIN    GPIO_NUM_3
#define DHT_PIN             GPIO_NUM_1
#define PIR_PIN             GPIO_NUM_2
#define RGB_LED_PIN         GPIO_NUM_48  // 4 NeoPixels are on this pin
#define RGB_LED_NUM         4            // Number of LEDs in the strip

ESP32Hardware::ESP32Hardware() {
    // 1. Setup Buzzer
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << ALARM_BUZZER_PIN);
    gpio_config(&io_conf);

    // 2. Setup PIR Sensor
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIR_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_config(&io_conf);

    // 3. Setup DHT22
    dht = new DHT22(DHT_PIN);

    // 4. Setup NeoPixel LED Strip (4 LEDs)
    led_strip_config_t strip_conf = {};
    strip_conf.strip_gpio_num = RGB_LED_PIN;
    strip_conf.max_leds = RGB_LED_NUM;
    strip_conf.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB; // WS2812 uses GRB order
    strip_conf.led_model = LED_MODEL_WS2812;

    led_strip_rmt_config_t rmt_conf = {};
    rmt_conf.resolution_hz = 10 * 1000 * 1000; // 10MHz

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_conf, &rmt_conf, &led_strip));
    led_strip_clear(led_strip);

    ESP_LOGI(TAG, "Hardware initialized (Buzzer:P3, DHT:P1, PIR:P2, 4-RGB LEDs:P48).");
}

void ESP32Hardware::setLedMode(LedMode mode) {
    uint32_t r = 0, g = 0, b = 0;
    switch (mode) {
        case LedMode::NORMAL:    g = 100; ESP_LOGI(TAG, "LEDs -> GREEN"); break;
        case LedMode::WARNING:   r = 100; g = 50; ESP_LOGI(TAG, "LEDs -> ORANGE"); break;
        case LedMode::EMERGENCY: r = 255; ESP_LOGW(TAG, "LEDs -> RED ALERT"); break;
        case LedMode::OFF:       break;
    }

    for (int i = 0; i < RGB_LED_NUM; i++) {
        led_strip_set_pixel(led_strip, i, r, g, b);
    }
    led_strip_refresh(led_strip);
}

void ESP32Hardware::setAlarm(bool active) {
    gpio_set_level(ALARM_BUZZER_PIN, active ? 1 : 0);
    if (active) ESP_LOGW(TAG, "ALARM ACTIVATED!");
}

float ESP32Hardware::getTemperature() {
    dht->read();
    return dht->getTemperature();
}

float ESP32Hardware::getHumidity() {
    return dht->getHumidity();
}

bool ESP32Hardware::isMotionDetected() {
    return gpio_get_level(PIR_PIN) == 1;
}

unsigned long ESP32Hardware::getMillis() {
    return (unsigned long)(esp_timer_get_time() / 1000);
}

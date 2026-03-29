#include "ESP32Hardware.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char* TAG = "ESP32Hardware";
// Pin definitions
#define ALARM_BUZZER_PIN    GPIO_NUM_3
#define STATUS_LED_PIN      GPIO_NUM_48 
#define DHT_PIN             GPIO_NUM_1
#define PIR_PIN             GPIO_NUM_2

ESP32Hardware::ESP32Hardware() {
    // 1. Setup Buzzer
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << ALARM_BUZZER_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // 2. Setup PIR Sensor
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << PIR_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // 3. Setup DHT22
    dht = new DHT22(DHT_PIN);

    ESP_LOGI(TAG, "Hardware initialized successfully (Buzzer: P3, DHT: P1, PIR: P2).");
}

void ESP32Hardware::setLedMode(LedMode mode) {
    switch (mode) {
        case LedMode::NORMAL:
            ESP_LOGI(TAG, "LED -> GREEN (Stable)");
            break;
        case LedMode::WARNING:
            ESP_LOGI(TAG, "LED -> ORANGE (Warning)");
            break;
        case LedMode::EMERGENCY:
            ESP_LOGW(TAG, "LED -> FLASHING RED (Emergency)");
            break;
        case LedMode::OFF:
            ESP_LOGI(TAG, "LED -> OFF");
            break;
    }
}

void ESP32Hardware::setAlarm(bool active) {
    gpio_set_level(ALARM_BUZZER_PIN, active ? 1 : 0);
    if (active) {
        ESP_LOGW(TAG, "ALARM ACTIVATED!");
    } else {
        ESP_LOGI(TAG, "Alarm deactivated.");
    }
}

float ESP32Hardware::getTemperature() {
    // Thêm log để debug kẹt ở đâu
    ESP_LOGD(TAG, "Reading DHT22...");
    if (!dht->read()) {
        ESP_LOGW(TAG, "DHT22 Read Failed! Using last known temperature.");
    }
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

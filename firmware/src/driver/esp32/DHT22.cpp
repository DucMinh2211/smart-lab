#include "DHT22.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "DHT22_DRIVER";

DHT22::DHT22(gpio_num_t gpio_pin) : pin(gpio_pin), temperature(0), humidity(0) {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
}

int DHT22::wait_for_level(int level, int timeout_us) {
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(pin) != level) {
        if (esp_timer_get_time() - start > timeout_us) return -1;
    }
    return (int)(esp_timer_get_time() - start);
}

bool DHT22::read() {
    uint8_t data[5] = {0, 0, 0, 0, 0};

    // 1. Send Start Signal
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
    ets_delay_us(18000); // 18ms Start pulse
    
    gpio_set_level(pin, 1);
    ets_delay_us(40);    // 40us Wait
    
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    // 2. Read 40 bits (Disable interrupts for precise timing)
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    taskENTER_CRITICAL(&mux);

    // Initial sequence: Low(80us) -> High(80us) -> Start Data
    if (wait_for_level(0, 100) < 0 || wait_for_level(1, 100) < 0 || wait_for_level(0, 100) < 0) {
        taskEXIT_CRITICAL(&mux);
        ESP_LOGW(TAG, "Sensor handshake failed!");
        return false;
    }

    for (int i = 0; i < 40; i++) {
        // Each bit starts with a ~50us Low, then a High pulse
        if (wait_for_level(1, 100) < 0) { taskEXIT_CRITICAL(&mux); return false; }
        
        // Measure the length of the High pulse
        int64_t t_start = esp_timer_get_time();
        if (wait_for_level(0, 100) < 0) { taskEXIT_CRITICAL(&mux); return false; }
        int64_t high_duration = esp_timer_get_time() - t_start;

        data[i / 8] <<= 1;
        // DHT22: '0' = 26us High, '1' = 70us High. Threshold = 45us.
        if (high_duration > 45) {
            data[i / 8] |= 1;
        }
    }
    taskEXIT_CRITICAL(&mux);

    // 3. Verify Checksum
    uint8_t expected = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
    if (data[4] != expected) {
        ESP_LOGW(TAG, "Checksum Error! Expected %02X, got %02X", expected, data[4]);
        return false;
    }

    // 5. Success - Decode Data
    humidity = ((data[0] << 8) | data[1]) / 10.0f;
    temperature = (((data[2] & 0x7F) << 8) | data[3]) / 10.0f;
    if (data[2] & 0x80) temperature *= -1;

    ESP_LOGD(TAG, "Read Success: T=%.1f H=%.1f", temperature, humidity);
    return true;
}

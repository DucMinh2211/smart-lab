#include "DHT20.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char* TAG = "DHT20";

DHT20::DHT20(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr) : temperature(0), humidity(0) {
    i2c_device_config_t dev_cfg = {};
    dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    dev_cfg.device_address = dev_addr;
    dev_cfg.scl_speed_hz = 100000; // 100kHz

    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    
    // Initial wait for sensor to stabilize
    vTaskDelay(pdMS_TO_TICKS(100));
}

bool DHT20::read() {
    uint8_t trigger_cmd[] = {0xAC, 0x33, 0x00};
    uint8_t data[6];

    // 1. Send trigger measurement command
    if (i2c_master_transmit(dev_handle, trigger_cmd, sizeof(trigger_cmd), 100) != ESP_OK) {
        return false;
    }

    // 2. Wait for measurement (min 80ms)
    vTaskDelay(pdMS_TO_TICKS(100));

    // 3. Read 6 bytes of data
    if (i2c_master_receive(dev_handle, data, sizeof(data), 100) != ESP_OK) {
        return false;
    }

    // 4. Check status bit (bit 7 must be 0 - not busy)
    if (data[0] & 0x80) {
        ESP_LOGW(TAG, "Sensor busy");
        return false;
    }

    // 5. Decode data (Standard AHT20/DHT20 formula)
    uint32_t hum_raw = ((uint32_t)data[1] << 12) | ((uint32_t)data[2] << 4) | (data[3] >> 4);
    uint32_t temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | data[5];

    humidity = (float)hum_raw * 100.0f / 1048576.0f;
    temperature = (float)temp_raw * 200.0f / 1048576.0f - 50.0f;

    return true;
}

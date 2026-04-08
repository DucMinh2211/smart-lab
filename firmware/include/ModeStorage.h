#ifndef MODE_STORAGE_H
#define MODE_STORAGE_H

#include "IHardware.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

/**
 * @brief NVS Storage for Operation Mode and Settings persistence
 * Saves/loads current operation mode and auto-light timer to survive reboots
 */
class ModeStorage {
private:
    static constexpr const char* TAG = "ModeStorage";
    static constexpr const char* NVS_NAMESPACE = "smartlab";
    static constexpr const char* MODE_KEY = "op_mode";
    static constexpr const char* TIMER_KEY = "light_timer";
    static constexpr unsigned long DEFAULT_TIMER = 60000; // 60 seconds default

public:
    /**
     * @brief Initialize NVS flash (call once at boot)
     */
    static esp_err_t init() {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        return ret;
    }

    /**
     * @brief Save operation mode to NVS
     */
    static esp_err_t saveMode(OperationMode mode) {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
            return err;
        }

        uint8_t modeValue = static_cast<uint8_t>(mode);
        err = nvs_set_u8(handle, MODE_KEY, modeValue);
        if (err == ESP_OK) {
            err = nvs_commit(handle);
            ESP_LOGI(TAG, "Saved mode: %s", 
                mode == OperationMode::GUEST_TAKING ? "GUEST_TAKING" : "SENTINEL");
        }

        nvs_close(handle);
        return err;
    }

    /**
     * @brief Load operation mode from NVS
     * @param defaultMode Default mode if no saved value exists
     */
    static OperationMode loadMode(OperationMode defaultMode = OperationMode::SENTINEL) {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "NVS not found, using default mode: SENTINEL");
            return defaultMode;
        }

        uint8_t modeValue;
        err = nvs_get_u8(handle, MODE_KEY, &modeValue);
        nvs_close(handle);

        if (err == ESP_OK) {
            OperationMode mode = static_cast<OperationMode>(modeValue);
            ESP_LOGI(TAG, "Loaded mode: %s", 
                mode == OperationMode::GUEST_TAKING ? "GUEST_TAKING" : "SENTINEL");
            return mode;
        } else {
            ESP_LOGW(TAG, "Mode key not found, using default: SENTINEL");
            return defaultMode;
        }
    }

    /**
     * @brief Save auto-light duration to NVS (in milliseconds)
     */
    static esp_err_t saveAutoLightDuration(unsigned long durationMs) {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
            return err;
        }

        err = nvs_set_u32(handle, TIMER_KEY, (uint32_t)durationMs);
        if (err == ESP_OK) {
            err = nvs_commit(handle);
            ESP_LOGI(TAG, "Saved auto-light timer: %lu ms", durationMs);
        }

        nvs_close(handle);
        return err;
    }

    /**
     * @brief Load auto-light duration from NVS
     * @return Duration in milliseconds (default 60000 if not found)
     */
    static unsigned long loadAutoLightDuration() {
        nvs_handle_t handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "NVS not found, using default timer: %lu ms", DEFAULT_TIMER);
            return DEFAULT_TIMER;
        }

        uint32_t durationMs;
        err = nvs_get_u32(handle, TIMER_KEY, &durationMs);
        nvs_close(handle);

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Loaded auto-light timer: %lu ms", (unsigned long)durationMs);
            return (unsigned long)durationMs;
        } else {
            ESP_LOGW(TAG, "Timer key not found, using default: %lu ms", DEFAULT_TIMER);
            return DEFAULT_TIMER;
        }
    }
};

#endif // MODE_STORAGE_H

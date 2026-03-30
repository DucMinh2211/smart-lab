#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#include "logic/SmartLabCore.h"
#include "ESP32Hardware.h"

static const char* TAG = "MAIN";

// Global handles
QueueHandle_t sensorQueue;
SmartLabCore* appLogic;
ESP32Hardware* hardware;

// Task: Read sensors (DHT22, PIR)
void sense_task(void* pvParameters) {
    ESP_LOGI(TAG, "Sense Task loop started.");
    while (1) {
        ESP_LOGI(TAG, "Tick...");
        // Read actual data from hardware
        float temp = hardware->getTemperature();
        float hum = hardware->getHumidity();
        bool motion = hardware->isMotionDetected();

        ESP_LOGI(TAG, "Sensed: Temp=%.1f C, Hum=%.1f %%, Motion=%s", 
                 temp, hum, motion ? "YES" : "NO");

        // Process logic
        appLogic->handleAutoLogic(temp, hum, motion);

        vTaskDelay(pdMS_TO_TICKS(2000)); // Read every 2 seconds
    }
}

// Task: Emergency (PIR)
void emergency_task(void* pvParameters) {
    ESP_LOGI(TAG, "Emergency Task started.");
    while (1) {
        // This will be triggered by a Semaphore from an ISR (Interrupt)
        // For now, we poll or wait for signal
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting Smart-Lab Sentinel...");

    // 1. Initialize Hardware & Logic
    hardware = new ESP32Hardware();
    appLogic = new SmartLabCore(hardware);

    // 2. Create Tasks
    xTaskCreatePinnedToCore(sense_task, "sense_task", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(emergency_task, "emergency_task", 4096, NULL, 4, NULL, 1);

    ESP_LOGI(TAG, "System up and running.");
}

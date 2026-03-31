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
SmartLabCore* appLogic;
ESP32Hardware* hardware;
SemaphoreHandle_t emergencySemaphore;

// ISR Handler: Runs when the PIR pin voltage transitions to HIGH
static void IRAM_ATTR pir_isr_handler(void* arg) {
    // Release Semaphore to wake up the emergency_task
    xSemaphoreGiveFromISR(emergencySemaphore, NULL);
}

// Task: Read sensors (DHT22/20) - Periodic
void sense_task(void* pvParameters) {
    ESP_LOGI(TAG, "Sense Task started.");
    while (1) {
        float temp = hardware->getTemperature();
        float hum = hardware->getHumidity();
        // PIR is now handled by Interrupt, but we still pass status for logging/logic
        bool motion = hardware->isMotionDetected();

        ESP_LOGI(TAG, "Sensed: Temp=%.1f C, Hum=%.1f %%", temp, hum);
        appLogic->handleAutoLogic(temp, hum, motion);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Task: Emergency (PIR) - Event Driven
void emergency_task(void* pvParameters) {
    ESP_LOGI(TAG, "Emergency Task waiting for trigger...");
    while (1) {
        // Wait indefinitely until a Semaphore is received from the ISR
        if (xSemaphoreTake(emergencySemaphore, portMAX_DELAY)) {
            ESP_LOGW(TAG, "!!! MOTION DETECTED INTERRUPT !!!");
            
            // Immediate alarm processing (Manual Override to Emergency)
            appLogic->handleManualCommand(LedMode::EMERGENCY);
            
            // Simple Debounce: Wait 2s before accepting the next interrupt
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting Smart-Lab Sentinel (Real-time ISR Mode)...");

    // 1. Initialize Hardware & Logic
    hardware = new ESP32Hardware();
    appLogic = new SmartLabCore(hardware);

    // 2. Setup Real-time Interrupts
    emergencySemaphore = xSemaphoreCreateBinary();
    
    // Install global GPIO ISR service
    gpio_install_isr_service(0);
    // Bind PIR pin (GPIO 2) to the handler
    gpio_isr_handler_add(GPIO_NUM_2, pir_isr_handler, NULL);

    // 3. Create Tasks
    xTaskCreatePinnedToCore(sense_task, "sense_task", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(emergency_task, "emergency_task", 4096, NULL, 5, NULL, 1);

    ESP_LOGI(TAG, "System up and running with ISR support.");
}

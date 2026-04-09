#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_vfs_dev.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"

#include "SmartLabCore.h"
#include "ESP32Hardware.h"
#include "ModeStorage.h"

static const char* TAG = "MAIN";

// Global handles
SmartLabCore* appLogic;
ESP32Hardware* hardware;
SemaphoreHandle_t emergencySemaphore;
SemaphoreHandle_t modeButtonSemaphore;

// Command buffer for Serial input
#define CMD_BUF_SIZE 64

// ISR Handler: Runs when the PIR pin voltage transitions to HIGH
static void IRAM_ATTR pir_isr_handler(void* arg) {
    // Release Semaphore to wake up the emergency_task
    xSemaphoreGiveFromISR(emergencySemaphore, NULL);
}

// ISR Handler: Runs when mode button is pressed
static void IRAM_ATTR mode_button_isr_handler(void* arg) {
    // Release Semaphore to wake up the mode_button_task
    xSemaphoreGiveFromISR(modeButtonSemaphore, NULL);
}

// Task: Read sensors (DHT22/20) - Periodic
void sense_task(void* pvParameters) {
    ESP_LOGI(TAG, "Sense Task started.");
    while (1) {
        float temp = hardware->getTemperature();
        float hum = hardware->getHumidity();
        // PIR is now handled by Interrupt, but we still pass status for logging/logic
        bool motion = hardware->isMotionDetected();

        OperationMode currentMode = appLogic->getOperationMode();
        const char* modeStr = (currentMode == OperationMode::GUEST_TAKING) ? "GUEST" : "SENTINEL";
        
        ESP_LOGI(TAG, "[%s] Temp=%.1f C, Hum=%.1f %%", modeStr, temp, hum);
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
            
            // Behavior depends on current mode
            OperationMode currentMode = appLogic->getOperationMode();
            if (currentMode == OperationMode::SENTINEL) {
                // Sentinel mode: Trigger emergency alarm immediately
                ESP_LOGW(TAG, "SENTINEL MODE: Triggering emergency alarm!");
                appLogic->triggerEmergency();
            } else {
                // Guest mode: Just turn on lights (handled by sense_task auto logic)
                ESP_LOGI(TAG, "GUEST MODE: Motion detected (auto-light will handle)");
            }
            
            // Simple Debounce: Wait 2s before accepting the next interrupt
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }
}

// Task: Mode Button Handler - Event Driven
void mode_button_task(void* pvParameters) {
    ESP_LOGI(TAG, "Mode Button Task started. Press button to toggle mode.");
    while (1) {
        // Wait for button press interrupt
        if (xSemaphoreTake(modeButtonSemaphore, portMAX_DELAY)) {
            // Disable button interrupt temporarily to prevent bounce
            gpio_intr_disable((gpio_num_t)MODE_BUTTON_PIN);
            
            // Clear any pending semaphores from bouncing
            while (xSemaphoreTake(modeButtonSemaphore, 0) == pdTRUE) {}
            
            // Toggle mode
            OperationMode currentMode = appLogic->getOperationMode();
            OperationMode newMode = (currentMode == OperationMode::GUEST_TAKING) 
                ? OperationMode::SENTINEL 
                : OperationMode::GUEST_TAKING;
            
            const char* newModeStr = (newMode == OperationMode::GUEST_TAKING) ? "GUEST_TAKING" : "SENTINEL";
            ESP_LOGI(TAG, "[<=>] Button pressed! Switching to %s mode", newModeStr);
            
            appLogic->setOperationMode(newMode);
            ModeStorage::saveMode(newMode);
            
            // Visual feedback: Flash LED
            hardware->setLedMode(LedMode::WARNING);
            vTaskDelay(pdMS_TO_TICKS(300));
            hardware->setLedMode(LedMode::OFF);
            
            ESP_LOGI(TAG, "[X] Mode switched and saved!");
            
            // Long debounce: Wait 3s before re-enabling interrupt
            vTaskDelay(pdMS_TO_TICKS(3000));
            
            // Re-enable button interrupt
            gpio_intr_enable((gpio_num_t)MODE_BUTTON_PIN);
        }
    }
}

// Task: Command handler (Serial/UART for testing)
void command_task(void* pvParameters) {
    ESP_LOGI(TAG, "Command Task started. Type commands (guest, sentinel, status, timer).");
    
    char line_buf[CMD_BUF_SIZE];
    int line_pos = 0;

    while (1) {
        uint8_t ch;
        // Read one byte at a time
        int len = uart_read_bytes(UART_NUM_0, &ch, 1, pdMS_TO_TICKS(10));
        
        if (len > 0) {
            if (ch == '\n' || ch == '\r') {
                // Echo newline to move cursor down on laptop screen
                uart_write_bytes(UART_NUM_0, "\r\n", 2);

                if (line_pos > 0) {
                    line_buf[line_pos] = '\0';
                    // Execute logic
                    if (strcmp(line_buf, "guest") == 0) {
                        appLogic->setOperationMode(OperationMode::GUEST_TAKING);
                        ModeStorage::saveMode(OperationMode::GUEST_TAKING);
                        ESP_LOGI(TAG, ">> Mode: GUEST");
                    } 
                    else if (strcmp(line_buf, "sentinel") == 0) {
                        appLogic->setOperationMode(OperationMode::SENTINEL);
                        ModeStorage::saveMode(OperationMode::SENTINEL);
                        ESP_LOGI(TAG, ">> Mode: SENTINEL");
                    }
                    else if (strcmp(line_buf, "status") == 0) {
                        OperationMode mode = appLogic->getOperationMode();
                        const char* modeStr = (mode == OperationMode::GUEST_TAKING) ? "GUEST" : "SENTINEL";
                        ESP_LOGI(TAG, "Status: %s", modeStr);
                    }
                    else if (strncmp(line_buf, "timer ", 6) == 0) {
                        int seconds = atoi(line_buf + 6);
                        if (seconds > 0) {
                            unsigned long durationMs = seconds * 1000;
                            appLogic->setAutoLightDuration(durationMs);
                            ModeStorage::saveAutoLightDuration(durationMs);
                            ESP_LOGI(TAG, ">> Timer: %d s", seconds);
                        }
                    }
                    else if (strncmp(line_buf, "volume ", 7) == 0) {
                        int vol = atoi(line_buf + 7);
                        hardware->setAlarmVolume(vol);
                        ESP_LOGI(TAG, ">> Volume set to %d%%", vol);
                    }
                    else {
                        ESP_LOGW(TAG, "!! Unknown: %s", line_buf);
                    }
                    line_pos = 0; // Reset
                }
            } 
            else if (ch == '\b' || ch == 127) { // Support Backspace/Delete
                if (line_pos > 0) {
                    line_pos--;
                    uart_write_bytes(UART_NUM_0, "\b \b", 3);
                }
            }
            else {
                // ECHO: Send the character back to your laptop screen
                if (line_pos < CMD_BUF_SIZE - 1) {
                    uart_write_bytes(UART_NUM_0, &ch, 1);
                    line_buf[line_pos++] = (char)ch;
                }
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

extern "C" void app_main() {
    ESP_LOGI(TAG, "Starting Smart-Lab Sentinel (Dual-Mode System)...");

    // 1. Initialize NVS
    ESP_ERROR_CHECK(ModeStorage::init());
    
    // 2. Initialize UART for Serial commands
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
        .flags = {
            .allow_pd = 0,
            .backup_before_sleep = 0
        },
    };
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 1024, 0, 0, NULL, 0));
    
    // Tell VFS to use the UART driver for standard I/O (essential for combined logging/input)
    uart_vfs_dev_use_driver(UART_NUM_0);
    
    // 3. Load saved operation mode and settings
    OperationMode savedMode = ModeStorage::loadMode();
    unsigned long savedTimer = ModeStorage::loadAutoLightDuration();
    
    const char* modeStr = (savedMode == OperationMode::GUEST_TAKING) ? "GUEST_TAKING" : "SENTINEL";
    ESP_LOGI(TAG, "Starting in %s mode", modeStr);
    ESP_LOGI(TAG, "Auto-light timer: %lu seconds", savedTimer / 1000);

    // 4. Initialize Hardware & Logic
    hardware = new ESP32Hardware();
    appLogic = new SmartLabCore(hardware, savedMode);
    appLogic->setAutoLightDuration(savedTimer);

    // 5. Setup Real-time Interrupts
    emergencySemaphore = xSemaphoreCreateBinary();
    modeButtonSemaphore = xSemaphoreCreateBinary();
    
    // Install global GPIO ISR service
    gpio_install_isr_service(0);
    
    // Bind PIR pin to ISR handler
    gpio_isr_handler_add((gpio_num_t)PIR_PIN, pir_isr_handler, NULL);
    
    // Setup Mode Button GPIO
    gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << MODE_BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // Trigger on button press (falling edge)
    };
    gpio_config(&btn_config);
    gpio_isr_handler_add((gpio_num_t)MODE_BUTTON_PIN, mode_button_isr_handler, NULL);

    // 6. Create Tasks
    xTaskCreatePinnedToCore(sense_task, "sense_task", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(emergency_task, "emergency_task", 4096, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(mode_button_task, "mode_button_task", 4096, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(command_task, "command_task", 4096, NULL, 1, NULL, 1);

    ESP_LOGI(TAG, "System up and running with Dual-Mode support.");
    ESP_LOGI(TAG, "[X] Press MODE button (A) to toggle between Guest/Sentinel modes.");
    ESP_LOGI(TAG, "[?] Serial commands: guest, sentinel, status, timer <seconds>");
}

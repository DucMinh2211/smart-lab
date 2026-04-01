#ifndef ESP32HARDWARE_H
#define ESP32HARDWARE_H

#include "IHardware.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "DHT22.h"
#include "DHT20.h"
#include "led_strip.h"
#include "ITempHumSensor.h"

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    // YOLO:BIT V3 (ESP32-S3) Pin Mapping
    #define I2C_SDA_PIN         GPIO_NUM_19  // P20
    #define I2C_SCL_PIN         GPIO_NUM_20  // P19
    #define ALARM_BUZZER_PIN    GPIO_NUM_38  // audio
    #define PIR_PIN             GPIO_NUM_1   // P0
    #define DHT22_PIN           GPIO_NUM_2   // P1
    #define RGB_LED_PIN         GPIO_NUM_15  // P14
#elif defined(CONFIG_IDF_TARGET_ESP32)
    // YOLO:BIT V2 (ESP32) Pin Mapping based on doc.md
    #define I2C_SDA_PIN         GPIO_NUM_21 // SDA
    #define I2C_SCL_PIN         GPIO_NUM_22 // SCL
    #define ALARM_BUZZER_PIN    GPIO_NUM_14 // Buzzer
    #define PIR_PIN             GPIO_NUM_26 // P0
    #define DHT22_PIN           GPIO_NUM_25 // P1
    #define RGB_LED_PIN         GPIO_NUM_23 // P14
#endif
#define RGB_LED_NUM         4

class ESP32Hardware : public IHardware {
private:
    ITempHumSensor* sensor; // Abstraction
    led_strip_handle_t led_strip;
    i2c_master_bus_handle_t i2c_bus;

public:
    ESP32Hardware();
    
    void setLedMode(LedMode mode) override;
    void setAlarm(bool active) override;
    
    float getTemperature() override;
    float getHumidity() override;
    bool isMotionDetected() override;
    
    unsigned long getMillis() override;
};

#endif // ESP32HARDWARE_H

#ifndef ESP32HARDWARE_H
#define ESP32HARDWARE_H

#include "IHardware.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "DHT22.h"
#include "DHT20.h"
#include "led_strip.h"

class ESP32Hardware : public IHardware {
private:
    DHT22* dht22;
    DHT20* dht20;
    led_strip_handle_t led_strip;
    i2c_master_bus_handle_t i2c_bus;
    
    bool use_dht20 = true; // Flag to toggle between sensors

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

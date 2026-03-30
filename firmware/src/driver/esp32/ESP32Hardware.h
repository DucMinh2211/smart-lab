#ifndef ESP32HARDWARE_H
#define ESP32HARDWARE_H

#include "IHardware.h"
#include "driver/gpio.h"
#include "DHT22.h"
#include "led_strip.h"

class ESP32Hardware : public IHardware {
private:
    DHT22* dht;
    led_strip_handle_t led_strip;

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

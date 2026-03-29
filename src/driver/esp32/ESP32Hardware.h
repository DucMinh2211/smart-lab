#ifndef ESP32HARDWARE_H
#define ESP32HARDWARE_H

#include "IHardware.h"
#include "driver/gpio.h"

#include "DHT22.h"

class ESP32Hardware : public IHardware {
private:
    DHT22* dht;

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

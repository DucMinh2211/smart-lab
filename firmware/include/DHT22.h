#ifndef DHT22_H
#define DHT22_H

#include "driver/gpio.h"
#include "ITempHumSensor.h"

class DHT22 : public ITempHumSensor {
private:
    gpio_num_t pin;
    float temperature;
    float humidity;

    int wait_for_level(int level, int timeout_us);

public:
    DHT22(gpio_num_t gpio_pin);
    bool read() override;
    float getTemperature() override { return temperature; }
    float getHumidity() override { return humidity; }
};

#endif // DHT22_H

#ifndef DHT22_H
#define DHT22_H

#include "driver/gpio.h"

class DHT22 {
private:
    gpio_num_t pin;
    float temperature;
    float humidity;

    int wait_for_level(int level, int timeout_us);

public:
    DHT22(gpio_num_t gpio_pin);
    
    /**
     * @brief Reads data from the sensor. 
     * @return true if reading was successful (checksum matches)
     */
    bool read();

    float getTemperature() const { return temperature; }
    float getHumidity() const { return humidity; }
};

#endif // DHT22_H

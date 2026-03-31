#ifndef DHT20_H
#define DHT20_H

#include "driver/i2c_master.h"

class DHT20 {
private:
    i2c_master_dev_handle_t dev_handle;
    float temperature;
    float humidity;

public:
    /**
     * @brief Constructor for DHT20/AHT20 I2C sensor
     * @param bus_handle The handle of the initialized I2C bus
     * @param dev_addr I2C address (Default for DHT20 is 0x38)
     */
    DHT20(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr = 0x38);
    
    /**
     * @brief Reads data from the I2C sensor.
     * @return true if reading was successful
     */
    bool read();

    float getTemperature() const { return temperature; }
    float getHumidity() const { return humidity; }
};

#endif // DHT20_H

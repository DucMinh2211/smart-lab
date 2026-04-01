#ifndef DHT20_H
#define DHT20_H

#include "driver/i2c_master.h"
#include "ITempHumSensor.h"

class DHT20 : public ITempHumSensor {
private:
    i2c_master_dev_handle_t dev_handle;
    float temperature;
    float humidity;

public:
    DHT20(i2c_master_bus_handle_t bus_handle, uint8_t dev_addr = 0x38);
    bool read() override;
    float getTemperature() override { return temperature; }
    float getHumidity() override { return humidity; }
};

#endif // DHT20_H

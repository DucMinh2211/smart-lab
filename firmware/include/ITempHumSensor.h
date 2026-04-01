#ifndef ITEMPHUMSENSOR_H
#define ITEMPHUMSENSOR_H

/**
 * @brief Common interface for Temperature and Humidity sensors
 */
class ITempHumSensor {
public:
    virtual ~ITempHumSensor() {}
    
    /**
     * @brief Trigger a sensor read
     * @return true if successful
     */
    virtual bool read() = 0;
    
    virtual float getTemperature() = 0;
    virtual float getHumidity() = 0;
};

#endif // ITEMPHUMSENSOR_H

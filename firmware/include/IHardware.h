#ifndef IHARDWARE_H
#define IHARDWARE_H

/**
 * @brief Defines display modes for the RGB LED
 */
enum class LedMode {
    OFF,
    NORMAL,    // Green: System stable
    WARNING,   // Orange: High temperature
    EMERGENCY  // Flashing Red: Intrusion detected
};

/**
 * @brief Hardware Abstraction Layer (HAL) interface
 * Decouples app logic from ESP32 SDK, enabling Unit Testing on PC.
 */
class IHardware {
public:
    virtual ~IHardware() {}

    /**
     * @brief Controls the RGB LED state
     */
    virtual void setLedMode(LedMode mode) = 0;

    /**
     * @brief Activates or deactivates the alarm or sends an emergency signal
     */
    virtual void setAlarm(bool active) = 0;

    /**
     * @brief Gets current temperature (°C)
     */
    virtual float getTemperature() = 0;

    /**
     * @brief Gets current humidity (%)
     */
    virtual float getHumidity() = 0;

    /**
     * @brief Checks if motion is detected (PIR)
     */
    virtual bool isMotionDetected() = 0;

    /**
     * @brief Gets system time (ms) for lockout duration calculations
     */
    virtual unsigned long getMillis() = 0;
};

#endif // IHARDWARE_H

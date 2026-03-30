#ifndef SMARTLABCORE_H
#define SMARTLABCORE_H

#include "IHardware.h"

class SmartLabCore {
private:
    IHardware* hw;
    unsigned long manualLockUntil = 0;
    const unsigned long LOCK_DURATION = 30000; // 30 giây lock-out
    LedMode currentMode = LedMode::OFF;
    bool isAlarming = false;

public:
    SmartLabCore(IHardware* hardware) : hw(hardware) {}

    /**
     * @brief Command from user (MQTT/Web) - Highest priority
     */
    void handleManualCommand(LedMode mode);

    /**
     * @brief Command from sensors (Auto) - Lower priority, blocked during lockout
     */
    void handleAutoLogic(float temp, float humidity, bool motion);

    /**
     * @brief For testing current state without hardware
     */
    LedMode getCurrentMode() const { return currentMode; }
    bool getIsAlarming() const { return isAlarming; }
};

#endif // SMARTLABCORE_H

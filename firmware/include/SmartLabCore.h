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
    OperationMode currentOperationMode = OperationMode::SENTINEL;
    
    // Auto-light timer for Guest mode
    unsigned long lastMotionTime = 0;
    unsigned long autoLightDuration = 60000; // 60 seconds default
    bool lightIsOn = false;

public:
    SmartLabCore(IHardware* hardware, OperationMode mode = OperationMode::SENTINEL) 
        : hw(hardware), currentOperationMode(mode) {}

    /**
     * @brief Command from user (MQTT/Web) - Highest priority, sets lockout
     */
    void handleManualCommand(LedMode mode);

    /**
     * @brief System-triggered emergency (PIR) - High priority, no lockout
     */
    void triggerEmergency();

    /**
     * @brief Command from sensors (Auto) - Lower priority, blocked during lockout
     */
    void handleAutoLogic(float temp, float humidity, bool motion);

    /**
     * @brief Switch operation mode (Guest-taking vs Sentinel)
     */
    void setOperationMode(OperationMode mode);

    /**
     * @brief Set auto-light duration for Guest mode (in milliseconds)
     * @param durationMs Duration in milliseconds before auto-turning off light
     */
    void setAutoLightDuration(unsigned long durationMs);

    /**
     * @brief Get current auto-light duration setting
     */
    unsigned long getAutoLightDuration() const { return autoLightDuration; }

    /**
     * @brief For testing current state without hardware
     */
    LedMode getCurrentMode() const { return currentMode; }
    bool getIsAlarming() const { return isAlarming; }
    OperationMode getOperationMode() const { return currentOperationMode; }

private:
    /**
     * @brief Guest mode: Comfort automation logic
     */
    void handleGuestModeLogic(float temp, float humidity, bool motion);

    /**
     * @brief Sentinel mode: Security monitoring logic
     */
    void handleSentinelModeLogic(float temp, float humidity, bool motion);
};

#endif // SMARTLABCORE_H

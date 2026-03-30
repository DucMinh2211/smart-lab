#include "SmartLabCore.h"

void SmartLabCore::handleManualCommand(LedMode mode) {
    // 1. Update state immediately
    currentMode = mode;
    hw->setLedMode(mode);

    // 2. Set the lockout release time
    // Auto commands are only allowed 30s after the last manual command.
    manualLockUntil = hw->getMillis() + LOCK_DURATION;
    
    // If switching to EMERGENCY manually (active alarm triggering)
    if (mode == LedMode::EMERGENCY) {
        isAlarming = true;
        hw->setAlarm(true);
    } else {
        isAlarming = false;
        hw->setAlarm(false);
    }
}

void SmartLabCore::handleAutoLogic(float temp, float humidity, bool motion) {
    // If still within the manual lockout period, skip automatic logic
    if (hw->getMillis() < manualLockUntil) {
        return;
    }

    // Automatic logic (Can be expanded with more rule engines here)
    LedMode newMode = LedMode::NORMAL;
    bool newAlarm = false;

    if (motion) {
        newMode = LedMode::EMERGENCY;
        newAlarm = true;
    } else if (temp > 35.0f) {
        newMode = LedMode::WARNING;
    } else {
        newMode = LedMode::NORMAL;
    }

    // Only update if state changes to save CPU/Resources
    if (newMode != currentMode) {
        currentMode = newMode;
        hw->setLedMode(currentMode);
    }

    if (newAlarm != isAlarming) {
        isAlarming = newAlarm;
        hw->setAlarm(isAlarming);
    }
}

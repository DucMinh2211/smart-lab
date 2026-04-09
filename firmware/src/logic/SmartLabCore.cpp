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

void SmartLabCore::triggerEmergency() {
    // Override current mode to EMERGENCY immediately without triggering lockout.
    // This allows the system to return to auto logic as soon as motion ends.
    currentMode = LedMode::EMERGENCY;
    hw->setLedMode(LedMode::EMERGENCY);
    isAlarming = true;
    hw->setAlarm(true);
}

void SmartLabCore::setOperationMode(OperationMode mode) {
    currentOperationMode = mode;
    // Reset alarm and LED when switching modes
    isAlarming = false;
    hw->setAlarm(false);
    currentMode = LedMode::OFF;
    hw->setLedMode(LedMode::OFF);
    // Reset lockout and timers
    manualLockUntil = 0;
    lastMotionTime = 0;
    lightIsOn = false;
}

void SmartLabCore::setAutoLightDuration(unsigned long durationMs) {
    autoLightDuration = durationMs;
}

void SmartLabCore::handleAutoLogic(float temp, float humidity, bool motion) {
    // If still within the manual lockout period, skip automatic logic
    if (hw->getMillis() < manualLockUntil) {
        return;
    }

    // Route to appropriate mode handler
    if (currentOperationMode == OperationMode::GUEST_TAKING) {
        handleGuestModeLogic(temp, humidity, motion);
    } else {
        handleSentinelModeLogic(temp, humidity, motion);
    }
}

void SmartLabCore::handleGuestModeLogic(float temp, float humidity, bool motion) {
    // Guest mode: Comfort automation (no security alarms)
    // Priority: Active motion > High temp > Light timer
    unsigned long currentTime = hw->getMillis();
    LedMode newMode = LedMode::OFF;
    bool newAlarm = false;

    if (motion) {
        // Motion detected → Turn on lights (motion has highest priority)
        newMode = LedMode::LIGHTING;
        lastMotionTime = currentTime;
        lightIsOn = true;
    } else if (temp > 30.0f || humidity > 60.0f) {
        // No motion but high temperature or humidity → Warning + fan
        newMode = LedMode::WARNING;
        newAlarm = true;
        lightIsOn = false; // Light off when in temp warning mode
    } else if (lightIsOn && (currentTime - lastMotionTime < autoLightDuration)) {
        // No motion, normal temp, timer not expired → Keep light on
        newMode = LedMode::LIGHTING;
    } else {
        // Normal conditions or timer expired → Lights off
        newMode = LedMode::OFF;
        lightIsOn = false;
    }

    // Only update if state changes
    if (newMode != currentMode) {
        currentMode = newMode;
        hw->setLedMode(currentMode);
    }

    if (newAlarm != isAlarming) {
        isAlarming = newAlarm;
        hw->setAlarm(isAlarming);
    }
}

void SmartLabCore::handleSentinelModeLogic(float temp, float humidity, bool motion) {
    // Sentinel mode: Security monitoring (original logic)
    LedMode newMode = LedMode::NORMAL;
    bool newAlarm = false;

    if (motion) {
        // Motion detected → Emergency alarm
        newMode = LedMode::EMERGENCY;
        newAlarm = true;
    } else if (temp > 35.0f || humidity > 70.0f) {
        // High temperature or humidity → Warning
        newMode = LedMode::WARNING;
    } else {
        // Normal monitoring
        newMode = LedMode::NORMAL;
    }

    // Only update if state changes
    if (newMode != currentMode) {
        currentMode = newMode;
        hw->setLedMode(currentMode);
    }

    if (newAlarm != isAlarming) {
        isAlarming = newAlarm;
        hw->setAlarm(isAlarming);
    }
}

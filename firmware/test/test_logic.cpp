#include <iostream>
#include <cassert>
#include <cstring>
#include "SmartLabCore.h"
#include "IHardware.h"

// ANSI Color codes for pretty output
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RESET   "\033[0m"

// Hardware simulation for logic testing
class MockHardware : public IHardware {
public:
    LedMode lastMode = LedMode::OFF;
    bool alarmActive = false;
    unsigned long currentMillis = 0;

    void setLedMode(LedMode mode) override { lastMode = mode; }
    void setAlarm(bool active) override { alarmActive = active; }
    
    // Mocking mandatory methods for IHardware
    float getTemperature() override { return 25.0f; }
    float getHumidity() override { return 60.0f; }
    bool isMotionDetected() override { return false; }
    
    unsigned long getMillis() override { return currentMillis; }
    
    // Reset state for new test
    void reset() {
        lastMode = LedMode::OFF;
        alarmActive = false;
        currentMillis = 0;
    }
};

void print_test_header(const char* testName) {
    std::cout << "\n" << COLOR_CYAN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << COLOR_RESET << "\n";
    std::cout << COLOR_YELLOW << "▶ " << testName << COLOR_RESET << "\n";
    std::cout << COLOR_CYAN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << COLOR_RESET << "\n";
}

void print_pass(const char* msg) {
    std::cout << COLOR_GREEN << "  ✓ " << msg << COLOR_RESET << "\n";
}

// ============================================================================
// TEST 1: Sentinel Mode - Motion Detection (Original Logic)
// ============================================================================
void test_sentinel_motion_detection() {
    print_test_header("TEST 1: Sentinel Mode - Motion Detection");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::SENTINEL);
    
    // Verify initial mode
    assert(core.getOperationMode() == OperationMode::SENTINEL);
    print_pass("Initial mode: SENTINEL");
    
    // Simulate motion detected
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);  // motion = true
    
    assert(hw.lastMode == LedMode::EMERGENCY);
    assert(hw.alarmActive == true);
    print_pass("Motion detected → EMERGENCY + Alarm ON");
    
    // No motion
    hw.currentMillis = 2000;
    core.handleAutoLogic(25.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::NORMAL);
    assert(hw.alarmActive == false);
    print_pass("No motion → NORMAL + Alarm OFF");
}

// ============================================================================
// TEST 2: Sentinel Mode - High Temperature
// ============================================================================
void test_sentinel_high_temperature() {
    print_test_header("TEST 2: Sentinel Mode - High Temperature");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::SENTINEL);
    
    // High temperature (>35°C)
    hw.currentMillis = 1000;
    core.handleAutoLogic(36.0, 60.0, false);  // temp = 36°C
    
    assert(hw.lastMode == LedMode::WARNING);
    assert(hw.alarmActive == false);
    print_pass("Temp 36°C → WARNING mode (no alarm)");
    
    // Normal temperature
    hw.currentMillis = 2000;
    core.handleAutoLogic(25.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::NORMAL);
    print_pass("Temp 25°C → NORMAL mode");
}

// ============================================================================
// TEST 3: Guest Mode - Motion Detection (Auto Light)
// ============================================================================
void test_guest_motion_auto_light() {
    print_test_header("TEST 3: Guest Mode - Motion Detection (Auto Light)");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Verify initial mode
    assert(core.getOperationMode() == OperationMode::GUEST_TAKING);
    print_pass("Initial mode: GUEST_TAKING");
    
    // Motion detected → Should turn on light (LIGHTING), NOT trigger alarm
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    
    // Debug: Check what mode we got
    if (hw.lastMode != LedMode::LIGHTING) {
        std::cerr << "ERROR: Expected LIGHTING, got mode " << static_cast<int>(hw.lastMode) << "\n";
        std::cerr << "OFF=" << static_cast<int>(LedMode::OFF) 
                  << ", NORMAL=" << static_cast<int>(LedMode::NORMAL)
                  << ", WARNING=" << static_cast<int>(LedMode::WARNING)
                  << ", EMERGENCY=" << static_cast<int>(LedMode::EMERGENCY)
                  << ", LIGHTING=" << static_cast<int>(LedMode::LIGHTING) << "\n";
    }
    
    assert(hw.lastMode == LedMode::LIGHTING);
    assert(hw.alarmActive == false);
    print_pass("Motion detected → LED LIGHTING (white light, NO alarm)");
    
    // No motion but within timer (default 60s) → Light STAYS on
    hw.currentMillis = 10000;  // 9s after motion
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    assert(hw.alarmActive == false);
    print_pass("No motion (within timer) → LED STAYS ON");
    
    // No motion after timer expires (>60s) → Light off
    hw.currentMillis = 65000;  // 64s after motion
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::OFF);
    assert(hw.alarmActive == false);
    print_pass("No motion (timer expired) → LED OFF");
}

// ============================================================================
// TEST 4: Guest Mode - High Temperature (Fan/Ventilation)
// ============================================================================
void test_guest_high_temperature_fan() {
    print_test_header("TEST 4: Guest Mode - High Temperature (Fan/Ventilation)");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // High temperature (>30°C) → Should activate warning + buzzer (as fan)
    hw.currentMillis = 1000;
    core.handleAutoLogic(31.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::WARNING);
    assert(hw.alarmActive == true);
    print_pass("Temp 31°C → WARNING + Buzzer ON (ventilation)");
    
    // Normal temperature
    hw.currentMillis = 2000;
    core.handleAutoLogic(28.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::OFF);
    assert(hw.alarmActive == false);
    print_pass("Temp 28°C → LED OFF + Buzzer OFF");
}

// ============================================================================
// TEST 5: Mode Switching
// ============================================================================
void test_mode_switching() {
    print_test_header("TEST 5: Mode Switching");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::SENTINEL);
    
    // Start in Sentinel
    assert(core.getOperationMode() == OperationMode::SENTINEL);
    print_pass("Initial: SENTINEL mode");
    
    // Set alarm manually
    core.handleManualCommand(LedMode::EMERGENCY);
    assert(hw.alarmActive == true);
    
    // Switch to Guest mode → Should reset alarm and LED
    core.setOperationMode(OperationMode::GUEST_TAKING);
    
    assert(core.getOperationMode() == OperationMode::GUEST_TAKING);
    assert(hw.alarmActive == false);
    assert(hw.lastMode == LedMode::OFF);
    print_pass("Switch to GUEST_TAKING → Alarm reset, LED OFF");
    
    // Switch back to Sentinel
    core.setOperationMode(OperationMode::SENTINEL);
    
    assert(core.getOperationMode() == OperationMode::SENTINEL);
    print_pass("Switch back to SENTINEL");
}

// ============================================================================
// TEST 6: Guest vs Sentinel - Different Behaviors
// ============================================================================
void test_different_behaviors_per_mode() {
    print_test_header("TEST 6: Guest vs Sentinel - Different Behaviors");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Guest mode: Motion → No alarm
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.alarmActive == false);
    print_pass("GUEST + Motion → No alarm");
    
    // Switch to Sentinel
    hw.reset();
    core.setOperationMode(OperationMode::SENTINEL);
    
    // Sentinel mode: Motion → Alarm
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.alarmActive == true);
    print_pass("SENTINEL + Motion → Alarm triggered");
}

// ============================================================================
// TEST 7: Manual Lockout (Works in both modes)
// ============================================================================
void test_lockout_in_both_modes() {
    print_test_header("TEST 7: Manual Lockout (Both Modes)");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Manual command → Lockout starts
    hw.currentMillis = 1000;
    core.handleManualCommand(LedMode::NORMAL);
    assert(hw.lastMode == LedMode::NORMAL);
    
    // Auto logic immediately after → Should be blocked
    hw.currentMillis = 2000;
    core.handleAutoLogic(32.0, 60.0, false);  // High temp
    
    assert(hw.lastMode == LedMode::NORMAL);  // Still NORMAL (locked)
    print_pass("Manual lockout: Auto command blocked");
    
    // Wait 31 seconds → Lockout expired
    hw.currentMillis = 32000;
    core.handleAutoLogic(32.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::WARNING);  // Now changed
    print_pass("After 30s: Auto command accepted");
}

// ============================================================================
// TEST 8: Guest Mode - Motion Priority over Temperature
// ============================================================================
void test_guest_motion_priority() {
    print_test_header("TEST 8: Guest Mode - Motion Priority");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Both motion AND high temp → Motion takes priority (light on)
    hw.currentMillis = 1000;
    core.handleAutoLogic(32.0, 60.0, true);
    
    assert(hw.lastMode == LedMode::LIGHTING);  // Motion → Lighting (not WARNING)
    assert(hw.alarmActive == false);  // No buzzer when motion present
    print_pass("Motion + High temp → LIGHTING (motion priority, no alarm)");
    
    // No motion but high temp persists → Now temp takes over
    hw.currentMillis = 70000;  // After timer expires
    core.handleAutoLogic(32.0, 60.0, false);
    
    assert(hw.lastMode == LedMode::WARNING);  // Temp → WARNING
    assert(hw.alarmActive == true);  // Buzzer ON for ventilation
    print_pass("No motion (timer expired) + High temp → WARNING + Buzzer");
}

// ============================================================================
// TEST 9: Guest Mode - Auto-Light Timer (Motion → Light stays on)
// ============================================================================
void test_guest_auto_light_timer() {
    print_test_header("TEST 9: Guest Mode - Auto-Light Timer");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Set short timer for testing (5 seconds)
    core.setAutoLightDuration(5000);
    assert(core.getAutoLightDuration() == 5000);
    print_pass("Timer set to 5 seconds");
    
    // Motion at t=1000ms → Light ON
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=1s: Motion → LED LIGHTING");
    
    // No motion at t=2000ms (1s after motion) → Light STILL ON (timer not expired)
    hw.currentMillis = 2000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=2s: No motion → LED still ON (timer active)");
    
    // No motion at t=4000ms (3s after motion) → Light STILL ON
    hw.currentMillis = 4000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=4s: No motion → LED still ON (timer active)");
    
    // No motion at t=6500ms (5.5s after motion) → Timer EXPIRED → Light OFF
    hw.currentMillis = 6500;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::OFF);
    print_pass("t=6.5s: Timer expired → LED OFF");
}

// ============================================================================
// TEST 10: Guest Mode - Timer Renewal on Motion
// ============================================================================
void test_guest_timer_renewal() {
    print_test_header("TEST 10: Guest Mode - Timer Renewal on Motion");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    core.setAutoLightDuration(5000);
    
    // First motion at t=1000ms
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=1s: First motion → LED LIGHTING");
    
    // No motion at t=3000ms (2s later) → Still ON
    hw.currentMillis = 3000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=3s: LED still ON");
    
    // NEW MOTION at t=5000ms → Timer RESETS from this point
    hw.currentMillis = 5000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=5s: Motion detected → Timer RESET");
    
    // No motion at t=8000ms (3s after LAST motion) → Still ON
    hw.currentMillis = 8000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=8s: LED still ON (new timer active)");
    
    // No motion at t=10500ms (5.5s after LAST motion) → Timer expired → OFF
    hw.currentMillis = 10500;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::OFF);
    print_pass("t=10.5s: Timer expired → LED OFF");
}

// ============================================================================
// TEST 11: Guest Mode - Timer Config Persistence
// ============================================================================
void test_guest_timer_config() {
    print_test_header("TEST 11: Guest Mode - Timer Configuration");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    
    // Default timer should be 60 seconds
    assert(core.getAutoLightDuration() == 60000);
    print_pass("Default timer: 60 seconds");
    
    // Set custom timer (30 seconds)
    core.setAutoLightDuration(30000);
    assert(core.getAutoLightDuration() == 30000);
    print_pass("Custom timer set: 30 seconds");
    
    // Verify it applies to logic
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    
    // 25 seconds later → Still ON
    hw.currentMillis = 26000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("t=26s: LED still ON (30s timer)");
    
    // 32 seconds later → Timer expired → OFF
    hw.currentMillis = 33000;
    core.handleAutoLogic(25.0, 60.0, false);
    assert(hw.lastMode == LedMode::OFF);
    print_pass("t=33s: Timer expired → LED OFF");
}

// ============================================================================
// TEST 12: Guest Mode - Timer vs Temperature Priority
// ============================================================================
void test_guest_timer_vs_temp() {
    print_test_header("TEST 12: Guest Mode - Timer vs Temperature");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    core.setAutoLightDuration(5000);
    
    // Motion at t=1000ms → Light ON
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("Motion → LED LIGHTING");
    
    // No motion but HIGH TEMP (>30°C) at t=3000ms
    // Should switch to WARNING (temp priority over light timer)
    hw.currentMillis = 3000;
    core.handleAutoLogic(32.0, 60.0, false);
    
    if (hw.lastMode != LedMode::WARNING) {
        std::cerr << "ERROR TEST 12: Expected WARNING, got mode " << static_cast<int>(hw.lastMode) << "\n";
        std::cerr << "  lightIsOn should be reset by temp, but got: " << (hw.lastMode == LedMode::LIGHTING ? "LIGHTING (timer)" : "other") << "\n";
    }
    
    assert(hw.lastMode == LedMode::WARNING);
    assert(hw.alarmActive == true);
    print_pass("High temp → WARNING mode (overrides timer)");
    
    // Temp returns to normal → Should be OFF (light timer doesn't apply in temp mode)
    hw.currentMillis = 4000;
    core.handleAutoLogic(28.0, 60.0, false);
    assert(hw.lastMode == LedMode::OFF);
    assert(hw.alarmActive == false);
    print_pass("Temp normal → LED OFF");
}

// ============================================================================
// TEST 13: Mode Switch Resets Timer
// ============================================================================
void test_mode_switch_resets_timer() {
    print_test_header("TEST 13: Mode Switch Resets Timer");
    
    MockHardware hw;
    SmartLabCore core(&hw, OperationMode::GUEST_TAKING);
    core.setAutoLightDuration(10000);
    
    // Motion in Guest mode
    hw.currentMillis = 1000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::LIGHTING);
    print_pass("GUEST mode: Motion → LED LIGHTING");
    
    // Switch to Sentinel mode → Should reset timer and turn off light
    core.setOperationMode(OperationMode::SENTINEL);
    assert(hw.lastMode == LedMode::OFF);
    print_pass("Switch to SENTINEL → LED OFF, timer reset");
    
    // Motion in Sentinel mode → EMERGENCY (not timer-based)
    hw.currentMillis = 2000;
    core.handleAutoLogic(25.0, 60.0, true);
    assert(hw.lastMode == LedMode::EMERGENCY);
    assert(hw.alarmActive == true);
    print_pass("SENTINEL mode: Motion → EMERGENCY");
}

// ============================================================================
// Main Test Runner
// ============================================================================
int main() {
    std::cout << "\n";
    std::cout << COLOR_CYAN << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║   Smart-Lab Dual-Mode Logic Test Suite      ║\n";
    std::cout << "║   Platform: Termux / Windows / Linux        ║\n";
    std::cout << "╚══════════════════════════════════════════════╝" << COLOR_RESET << "\n";
    
    try {
        // Original tests
        test_sentinel_motion_detection();
        test_sentinel_high_temperature();
        test_guest_motion_auto_light();
        test_guest_high_temperature_fan();
        test_mode_switching();
        test_different_behaviors_per_mode();
        test_lockout_in_both_modes();
        test_guest_motion_priority();
        
        // New auto-timer tests
        test_guest_auto_light_timer();
        test_guest_timer_renewal();
        test_guest_timer_config();
        test_guest_timer_vs_temp();
        test_mode_switch_resets_timer();
        
        std::cout << "\n" << COLOR_GREEN << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        std::cout << "✅ ALL 13 TESTS PASSED!\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << COLOR_RESET << "\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test failed with exception: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}

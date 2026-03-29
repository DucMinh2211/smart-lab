#include <iostream>
#include <cassert>
#include "logic/SmartLabCore.h"

// Hardware simulation for logic testing
class MockHardware : public IHardware {
public:
    LedMode lastMode = LedMode::OFF;
    bool alarmActive = false;
    unsigned long currentMillis = 0;

    void setLedMode(LedMode mode) override { lastMode = mode; }
    void setAlarm(bool active) override { alarmActive = active; }
    unsigned long getMillis() override { return currentMillis; }
};

void test_lockout_logic() {
    MockHardware hw;
    SmartLabCore core(&hw);

    std::cout << "Running test_lockout_logic..." << std::endl;

    // 1. Simulate user manually setting to NORMAL
    hw.currentMillis = 1000;
    core.handleManualCommand(LedMode::NORMAL);
    assert(hw.lastMode == LedMode::NORMAL);

    // 2. Immediately (at 2000ms), sensor detects intrusion (EMERGENCY)
    hw.currentMillis = 2000;
    core.handleAutoLogic(25.0, 60.0, true);
    
    // Result: Status remains NORMAL due to the 30s lockout
    // Automatic command (EMERGENCY) is ignored
    assert(hw.lastMode == LedMode::NORMAL);
    assert(core.getCurrentMode() == LedMode::NORMAL);
    std::cout << "  - Lock-out check: PASSED (Auto command ignored during lock)" << std::endl;

    // 3. Wait 31 seconds (32000ms)
    hw.currentMillis = 32000;
    core.handleAutoLogic(25.0, 60.0, true);
    
    // Result: Lockout expired, automatic command (EMERGENCY) must be executed
    assert(hw.lastMode == LedMode::EMERGENCY);
    assert(hw.alarmActive == true);
    std::cout << "  - Lock-out release: PASSED (Auto command accepted after 30s)" << std::endl;

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    try {
        test_lockout_logic();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

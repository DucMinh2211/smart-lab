# Smart-Lab Dual-Mode Logic Tests

## 🎯 Purpose
Comprehensive unit tests for Guest-taking vs Sentinel mode logic.
Tests run independently of ESP32 hardware on any platform.

## 🛠️ Build & Run

### Prerequisites
- C++17 compiler (g++, clang++, or MSVC)
- CMake 3.10+ (recommended) or Make

### Quick Start with CMake (Recommended)

#### All Platforms (Linux/macOS/Termux/Windows):
```bash
cd firmware/test
cmake -B build
cmake --build build
./build/test_logic          # Linux/macOS/Termux
# or
.\build\Debug\test_logic.exe   # Windows MSVC
# or
.\build\test_logic.exe         # Windows MinGW
```

#### One-liner:
```bash
cmake -B build && cmake --build build && ./build/test_logic
```

#### Using CTest:
```bash
cd build
ctest --output-on-failure
```

### Alternative: Using Make

**Linux/macOS/Termux/Windows with MinGW:**
```bash
make run
```

### Manual Compilation

**Linux/macOS/Termux:**
```bash
g++ -std=c++17 -Wall -Wextra -I../include \
    test_logic.cpp ../src/logic/SmartLabCore.cpp \
    -o test_logic
./test_logic
```

**Windows (PowerShell with MinGW):**
```powershell
g++ -std=c++17 -Wall -Wextra -I../include test_logic.cpp ../src/logic/SmartLabCore.cpp -o test_logic.exe
.\test_logic.exe
```

## 📋 Test Coverage

### Test Suite (13 Tests):
1. ✅ **Sentinel - Motion Detection**: Emergency alarm on motion
2. ✅ **Sentinel - High Temperature**: Warning mode at >35°C
3. ✅ **Guest - Motion Auto Light**: LED on when motion (no alarm)
4. ✅ **Guest - High Temp Fan**: Buzzer/fan at >30°C
5. ✅ **Mode Switching**: Clean transition between modes
6. ✅ **Different Behaviors**: Guest vs Sentinel logic comparison
7. ✅ **Manual Lockout**: 30s override protection in both modes
8. ✅ **Guest Motion Priority**: Motion overrides temp actions
9. ✅ **Guest Auto-Light Timer**: Light stays on for configured duration
10. ✅ **Guest Timer Renewal**: Motion resets timer countdown
11. ✅ **Timer Configuration**: Custom timer settings work correctly
12. ✅ **Timer vs Temperature**: Temp priority overrides light timer
13. ✅ **Mode Switch Resets Timer**: Timer clears on mode change

## 🎨 Output Example
```
╔══════════════════════════════════════════════╗
║   Smart-Lab Dual-Mode Logic Test Suite      ║
║   Platform: Termux / Windows / Linux        ║
╚══════════════════════════════════════════════╝

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
▶ TEST 1: Sentinel Mode - Motion Detection
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  ✓ Initial mode: SENTINEL
  ✓ Motion detected → EMERGENCY + Alarm ON
  ✓ No motion → NORMAL + Alarm OFF

...

✅ ALL 8 TESTS PASSED!
```

## 🔍 What's Being Tested

### Sentinel Mode (Security):
- Motion → EMERGENCY + Alarm
- Temp > 35°C → WARNING
- Normal → NORMAL monitoring

### Guest-taking Mode (Comfort):
- Motion → LED ON (NORMAL, no alarm)
- Temp > 30°C → WARNING + Buzzer (ventilation)
- No triggers → LED OFF

### Shared Behaviors:
- Manual commands override auto logic for 30s
- Mode switching resets alarm/LED state
- Clean state management

## 📝 Notes
- Tests use MockHardware (no ESP32 dependencies)
- All logic is platform-independent
- Color output works on Unix terminals (graceful fallback on Windows)
- CMake provides best cross-platform compatibility

cmake_minimum_required(VERSION 3.10)
project(SmartLabLogicTest CXX)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Get absolute paths
get_filename_component(FIRMWARE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
get_filename_component(INCLUDE_DIR "${FIRMWARE_DIR}/include" ABSOLUTE)
get_filename_component(SRC_DIR "${FIRMWARE_DIR}/src/logic" ABSOLUTE)

# Include directories
include_directories(${INCLUDE_DIR})

# Source files
set(TEST_SOURCES
    test_logic.cpp
    ${SRC_DIR}/SmartLabCore.cpp
)

# Executable
add_executable(test_logic ${TEST_SOURCES})

# Enable testing
enable_testing()
add_test(NAME DualModeLogicTest COMMAND test_logic)

# Print info
message(STATUS "====================================")
message(STATUS "Smart-Lab Dual-Mode Logic Test")
message(STATUS "====================================")
message(STATUS "Firmware dir: ${FIRMWARE_DIR}")
message(STATUS "Include dir: ${INCLUDE_DIR}")
message(STATUS "Source dir: ${SRC_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "====================================")

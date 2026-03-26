cmake_minimum_required(VERSION 3.10)
project(SmartLabLogicTest)

# Xuất file compile_commands.json cho LSP (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Cấu hình C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Thêm flag giả lập cho môi trường Desktop/Termux
add_definitions(-DDESKTOP_APP)

# Đường dẫn Header
include_directories(include)

# Tìm tất cả các file source trong src/ (ngoại trừ main của ESP-IDF nếu cần)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp" "test/*.c")

# Tạo file thực thi để chạy test
add_executable(run_tests ${SOURCES} ${TEST_SOURCES})

# Hiển thị thông báo khi build xong
add_custom_command(TARGET run_tests POST_BUILD
    COMMAND ./run_tests
    COMMENT "Running logic tests..."
)

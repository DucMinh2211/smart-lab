cmake_minimum_required(VERSION 3.16)

# Thêm các thư mục chứa linh kiện (components)
set(EXTRA_COMPONENT_DIRS 
    "src/logic"
    "src/driver/esp32"
    "include"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(smart-lab)

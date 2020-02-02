cmake_minimum_required(VERSION 3.16)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

project(lvgl)

file(GLOB_RECURSE INCLUDES "../lvgl/*.h")
file(GLOB_RECURSE SOURCES  "../lvgl/*.c")

add_library(lvgl STATIC ${SOURCES})

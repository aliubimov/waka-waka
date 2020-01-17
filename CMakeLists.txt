cmake_minimum_required(VERSION 3.10)
project(npx)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "lvgl/*.h" "./*.h")
file(GLOB_RECURSE SOURCES  "lv_drivers/*.c" "lvgl/*.c")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

find_package(SDL2 REQUIRED SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(main main.c ${SOURCES} ${INCLUDES})

target_link_libraries(main PRIVATE SDL2 )

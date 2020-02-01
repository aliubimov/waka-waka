cmake_minimum_required(VERSION 3.10)
project(npx)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "lvgl/*.h" "*.h")
file(GLOB_RECURSE SOURCES  "lv_drivers/*.c" "lvgl/*.c")

file(GLOB_RECURSE LORA_DRV_INCL  "lora_drv/*.h")
file(GLOB_RECURSE LORA_DRV_SRC  "lora_drv/*.c")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})

find_package(SDL2 REQUIRED SDL2)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(main main.cpp ${SOURCES} ${INCLUDES})
add_executable(spi_main spi_main.cpp ${SOURCES} ${INCLUDES})

target_link_libraries(main PRIVATE SDL2 )
target_link_libraries(main PRIVATE mpsse )
target_compile_options(main PRIVATE "-DLIBFTDI1=1")

add_executable(lora lora.cpp ${LORA_DRV_SRC} ${LORA_DRV_INCL})

target_link_libraries(lora PRIVATE mpsse )
target_compile_options(lora PRIVATE "-DLIBFTDI1=1")

target_link_libraries(spi_main PRIVATE SDL2 )
target_link_libraries(spi_main PRIVATE mpsse )
target_compile_options(spi_main PRIVATE "-DLIBFTDI1=1")

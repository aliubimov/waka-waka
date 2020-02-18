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

set(USER_INTERFACE
    "../source/app/waka.c"
    "../source/app/logo.c"
    "../source/app/waka_splash.c"
    "../source/app/waka_input_msg.c"
    )

add_executable(main main.cpp terminus.c ${USER_INTERFACE} ${SOURCES} ${INCLUDES})

target_link_libraries(main PRIVATE SDL2 )

add_executable(lora lora.cpp ${LORA_DRV_SRC} ${LORA_DRV_INCL})

target_link_libraries(lora PRIVATE mpsse )
target_compile_options(lora PRIVATE "-DLIBFTDI1=1")

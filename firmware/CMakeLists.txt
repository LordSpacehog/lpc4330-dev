cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_TOOLCHAIN_FILE toolchain-arm-cortex-m.cmake)

project (opendac_firmware_all)

add_subdirectory(blinky)

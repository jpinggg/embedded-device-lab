cmake_minimum_required(VERSION 3.16)

project(embedded_device_lab LANGUAGES CXX)

add_executable(device_version_demo
    src/device_version.cpp
    src/device_version_demo.cpp
)

target_include_directories(device_version_demo
    PRIVATE
    include
)

target_compile_features(device_version_demo PRIVATE cxx_std_17)

target_compile_options(device_version_demo
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

cmake_minimum_required(VERSION 3.16)

project(embedded_device_lab LANGUAGES CXX)
include(CTest)

add_library(device_version
    src/device_version.cpp
)

target_include_directories(device_version
    PUBLIC
    include
)

target_compile_features(device_version PUBLIC cxx_std_17)

target_compile_options(device_version
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_library(update_session
    src/update_session.cpp
)

target_include_directories(update_session
    PUBLIC
    include
)

target_link_libraries(update_session
    PUBLIC
    device_version
)

target_compile_features(update_session PUBLIC cxx_std_17)

target_compile_options(update_session
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(device_version_demo
    src/device_version_demo.cpp
)

target_link_libraries(device_version_demo
    PRIVATE
    device_version
)

target_compile_options(device_version_demo
    PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if(BUILD_TESTING)
    add_executable(device_version_test
        tests/device_version_test.cpp
    )

    target_link_libraries(device_version_test
        PRIVATE
        device_version
    )

    target_compile_options(device_version_test
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    add_test(
        NAME device_version_behavior
        COMMAND device_version_test
    )

    add_executable(update_session_test
        tests/update_session_test.cpp
    )

    target_link_libraries(update_session_test
        PRIVATE
        update_session
    )

    target_compile_options(update_session_test
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )

    add_test(
        NAME update_session_behavior
        COMMAND update_session_test
    )
endif()

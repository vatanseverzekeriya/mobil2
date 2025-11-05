cmake_minimum_required(VERSION 3.18)
project(Metin2Mobile VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform detection
if(ANDROID)
    set(M2_PLATFORM_ANDROID ON)
    set(M2_MOBILE ON)
elseif(IOS)
    set(M2_PLATFORM_IOS ON)
    set(M2_MOBILE ON)
elseif(WIN32)
    set(M2_PLATFORM_WINDOWS ON)
elseif(UNIX AND NOT APPLE)
    set(M2_PLATFORM_LINUX ON)
elseif(APPLE)
    set(M2_PLATFORM_MACOS ON)
endif()

# Compiler options
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_options(/MP) # Multi-processor compilation
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)

    # Optimization flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
endif()

# Mobile-specific optimizations
if(M2_MOBILE)
    add_compile_definitions(M2_MOBILE)

    if(M2_PLATFORM_ANDROID)
        add_compile_definitions(M2_PLATFORM_ANDROID)

        # ARM NEON optimizations
        if(ANDROID_ABI STREQUAL "arm64-v8a")
            add_compile_options(-march=armv8-a -mtune=cortex-a53)
        endif()
    elseif(M2_PLATFORM_IOS)
        add_compile_definitions(M2_PLATFORM_IOS)

        # iOS specific flags
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    endif()
endif()

# Debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(M2_DEBUG)

    # Sanitizers (non-mobile)
    if(NOT M2_MOBILE AND NOT MSVC)
        add_compile_options(-fsanitize=address -fsanitize=undefined)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# External dependencies
add_subdirectory(external)

# Core modules
add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(network)
add_subdirectory(entity)

# Optional modules
option(BUILD_COMBAT_MODULE "Build combat module" ON)
option(BUILD_WORLD_MODULE "Build world module" ON)
option(BUILD_QUEST_MODULE "Build quest module" ON)
option(BUILD_DATABASE_MODULE "Build database module" ON)
option(BUILD_UI_MODULE "Build UI module" ON)

if(BUILD_COMBAT_MODULE)
    add_subdirectory(combat)
endif()

if(BUILD_WORLD_MODULE)
    add_subdirectory(world)
endif()

if(BUILD_QUEST_MODULE)
    add_subdirectory(quest)
endif()

if(BUILD_DATABASE_MODULE)
    add_subdirectory(database)
endif()

if(BUILD_UI_MODULE)
    add_subdirectory(ui)
endif()

# Applications
option(BUILD_CLIENT "Build game client" ON)
option(BUILD_SERVER "Build game server" OFF)

if(BUILD_CLIENT)
    add_subdirectory(game-client)
endif()

if(BUILD_SERVER)
    add_subdirectory(game-server)
endif()

# Tests
option(BUILD_TESTS "Build unit tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS metin2_core metin2_platform metin2_network metin2_entity
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# CPack for packaging
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "========== Metin2 Mobile Configuration ==========")
message(STATUS "Build type:           ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:         ${CMAKE_CXX_STANDARD}")
message(STATUS "Platform:             ${CMAKE_SYSTEM_NAME}")
if(M2_MOBILE)
    message(STATUS "Mobile build:         YES")
endif()
if(M2_PLATFORM_ANDROID)
    message(STATUS "Android ABI:          ${ANDROID_ABI}")
    message(STATUS "Android API:          ${ANDROID_PLATFORM}")
endif()
message(STATUS "Build client:         ${BUILD_CLIENT}")
message(STATUS "Build server:         ${BUILD_SERVER}")
message(STATUS "Build tests:          ${BUILD_TESTS}")
message(STATUS "==================================================")
message(STATUS "")

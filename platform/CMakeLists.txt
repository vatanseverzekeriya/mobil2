cmake_minimum_required(VERSION 3.18)
project(metin2_platform)

# Common source files
set(PLATFORM_SOURCES
    src/common/file_system.cpp
)

# Platform-specific sources
if(M2_PLATFORM_ANDROID)
    list(APPEND PLATFORM_SOURCES
        src/android/android_platform.cpp
        src/android/android_file_system.cpp
    )
elseif(M2_PLATFORM_IOS)
    list(APPEND PLATFORM_SOURCES
        src/ios/ios_platform.mm
        src/ios/ios_file_system.mm
    )
elseif(M2_PLATFORM_WINDOWS)
    list(APPEND PLATFORM_SOURCES
        src/windows/windows_platform.cpp
        src/windows/windows_file_system.cpp
    )
elseif(M2_PLATFORM_LINUX)
    list(APPEND PLATFORM_SOURCES
        src/linux/linux_platform.cpp
        src/linux/linux_file_system.cpp
    )
endif()

# Header files
set(PLATFORM_HEADERS
    include/metin2/platform/platform.hpp
    include/metin2/platform/file_system.hpp
    include/metin2/platform/thread.hpp
    include/metin2/platform/network_info.hpp
)

# Create library
add_library(metin2_platform STATIC ${PLATFORM_SOURCES} ${PLATFORM_HEADERS})

# Include directories
target_include_directories(metin2_platform
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(metin2_platform
    PUBLIC
        metin2_core
)

# Platform-specific dependencies
if(M2_PLATFORM_ANDROID)
    target_link_libraries(metin2_platform
        PUBLIC
            android
            log
    )
elseif(M2_PLATFORM_IOS)
    target_link_libraries(metin2_platform
        PUBLIC
            "-framework Foundation"
            "-framework UIKit"
    )
elseif(M2_PLATFORM_WINDOWS)
    target_link_libraries(metin2_platform
        PUBLIC
            Shlwapi
    )
endif()

# Compiler features
target_compile_features(metin2_platform PUBLIC cxx_std_20)

# Installation
install(TARGETS metin2_platform
    EXPORT metin2_platform_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

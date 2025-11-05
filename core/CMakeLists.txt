cmake_minimum_required(VERSION 3.18)
project(metin2_core)

# Source files
set(CORE_SOURCES
    src/event_system.cpp
    src/memory_pool.cpp
    src/logger.cpp
)

# Header files
set(CORE_HEADERS
    include/metin2/core/types.hpp
    include/metin2/core/singleton.hpp
    include/metin2/core/event_system.hpp
    include/metin2/core/memory_pool.hpp
    include/metin2/core/logger.hpp
)

# Create library
add_library(metin2_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

# Include directories
target_include_directories(metin2_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link libraries
target_link_libraries(metin2_core
    PUBLIC
        # Threading support
        ${CMAKE_THREAD_LIBS_INIT}
)

# Find Threads package
find_package(Threads REQUIRED)
target_link_libraries(metin2_core PUBLIC Threads::Threads)

# Compiler features
target_compile_features(metin2_core PUBLIC cxx_std_20)

# Platform-specific
if(M2_PLATFORM_ANDROID)
    target_link_libraries(metin2_core PUBLIC log)
elseif(M2_PLATFORM_IOS)
    target_link_libraries(metin2_core PUBLIC "-framework Foundation")
endif()

# Installation
install(TARGETS metin2_core
    EXPORT metin2_core_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

# Tests
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

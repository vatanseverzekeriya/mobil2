cmake_minimum_required(VERSION 3.18)
project(metin2_network)

# Source files
set(NETWORK_SOURCES
    src/session.cpp
    src/packet.cpp
)

# Header files
set(NETWORK_HEADERS
    include/metin2/network/session.hpp
    include/metin2/network/packet.hpp
    include/metin2/network/protocol.hpp
)

# Create library
add_library(metin2_network STATIC ${NETWORK_SOURCES} ${NETWORK_HEADERS})

# Include directories
target_include_directories(metin2_network
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(metin2_network
    PUBLIC
        metin2_core
    PRIVATE
        # ASIO or platform networking
)

# Platform-specific networking
if(M2_PLATFORM_WINDOWS)
    target_link_libraries(metin2_network PRIVATE ws2_32 wsock32)
elseif(M2_PLATFORM_ANDROID OR M2_PLATFORM_LINUX)
    # Standard POSIX sockets
elseif(M2_PLATFORM_IOS)
    target_link_libraries(metin2_network PRIVATE "-framework CFNetwork")
endif()

# Compiler features
target_compile_features(metin2_network PUBLIC cxx_std_20)

# Installation
install(TARGETS metin2_network
    EXPORT metin2_network_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

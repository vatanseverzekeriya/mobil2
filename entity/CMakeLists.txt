cmake_minimum_required(VERSION 3.18)
project(metin2_entity)

# Source files
set(ENTITY_SOURCES
    src/entity.cpp
    src/character.cpp
    src/item.cpp
    src/inventory.cpp
    src/monster.cpp
)

# Header files
set(ENTITY_HEADERS
    include/metin2/entity/entity.hpp
    include/metin2/entity/component.hpp
    include/metin2/entity/character.hpp
    include/metin2/entity/item.hpp
    include/metin2/entity/inventory.hpp
    include/metin2/entity/monster.hpp
)

# Create library
add_library(metin2_entity STATIC ${ENTITY_SOURCES} ${ENTITY_HEADERS})

# Include directories
target_include_directories(metin2_entity
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link dependencies
target_link_libraries(metin2_entity
    PUBLIC
        metin2_core
        metin2_network
)

# Compiler features
target_compile_features(metin2_entity PUBLIC cxx_std_20)

# Installation
install(TARGETS metin2_entity
    EXPORT metin2_entity_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.hpp"
)

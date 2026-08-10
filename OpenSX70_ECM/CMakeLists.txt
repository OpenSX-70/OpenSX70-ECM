cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME OpenSX70_ECM)

option(OPENSX70_USE_BOOTLOADER
    "Link the application after an 8 KiB bootloader"
    OFF
)

if(OPENSX70_USE_BOOTLOADER)
    set(OPENSX70_FLASH_ORIGIN 0x08002000)
    set(OPENSX70_FLASH_LENGTH 0x0000D000) # 52 KiB after the 8 KiB bootloader
    set(OPENSX70_VECTOR_OFFSET 0x00002000)
else()
    set(OPENSX70_FLASH_ORIGIN 0x08000000)
    set(OPENSX70_FLASH_LENGTH 0x0000F000) # 60 KiB application maximum
    set(OPENSX70_VECTOR_OFFSET 0x00000000)
endif()

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Keep the linked application address and the runtime vector table in sync.
target_link_options(${CMAKE_PROJECT_NAME} PRIVATE
    "-Wl,--defsym=OPENSX70_FLASH_ORIGIN=${OPENSX70_FLASH_ORIGIN}"
    "-Wl,--defsym=OPENSX70_FLASH_LENGTH=${OPENSX70_FLASH_LENGTH}"
)
target_compile_definitions(STM32_Drivers PRIVATE
    USER_VECT_TAB_ADDRESS
    VECT_TAB_OFFSET=${OPENSX70_VECTOR_OFFSET}U
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/main.c
    Core/Src/opensx70.c
    Core/Src/camerafunctions.c
    Core/Src/settings.c
    Core/Src/meter.c
    Core/Src/peripheralport.c
    Core/Src/pollers.c
    Core/Src/shutterbutton.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)

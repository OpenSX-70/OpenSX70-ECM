set(CMAKE_SYSTEM_NAME               Generic)
set(CMAKE_SYSTEM_PROCESSOR          arm)

# Optional path to the GNU Arm Embedded Toolchain bin directory.
# Can also be set with env vars ARM_GCC_BIN or ARM_GCC_TOOLCHAIN.
set(ARM_GCC_BIN "" CACHE PATH "Path to GNU Arm Embedded Toolchain bin directory")

set(_arm_program_hints)
if(ARM_GCC_BIN)
	list(APPEND _arm_program_hints "${ARM_GCC_BIN}")
endif()

if(DEFINED ENV{ARM_GCC_BIN} AND NOT "$ENV{ARM_GCC_BIN}" STREQUAL "")
	list(APPEND _arm_program_hints "$ENV{ARM_GCC_BIN}")
endif()

if(DEFINED ENV{ARM_GCC_TOOLCHAIN} AND NOT "$ENV{ARM_GCC_TOOLCHAIN}" STREQUAL "")
	list(APPEND _arm_program_hints "$ENV{ARM_GCC_TOOLCHAIN}/bin")
endif()

list(APPEND CMAKE_PROGRAM_PATH ${_arm_program_hints})

find_program(ARM_NONE_EABI_GCC NAMES arm-none-eabi-gcc)
if(NOT ARM_NONE_EABI_GCC)
	message(FATAL_ERROR
		"Could not find arm-none-eabi-gcc. Install GNU Arm Embedded Toolchain, add it to PATH, "
		"or set ARM_GCC_BIN / ARM_GCC_TOOLCHAIN.")
endif()

get_filename_component(ARM_GCC_BIN_DIR "${ARM_NONE_EABI_GCC}" DIRECTORY)

find_program(ARM_NONE_EABI_GXX NAMES arm-none-eabi-g++ HINTS "${ARM_GCC_BIN_DIR}")
find_program(ARM_NONE_EABI_OBJCOPY NAMES arm-none-eabi-objcopy HINTS "${ARM_GCC_BIN_DIR}")
find_program(ARM_NONE_EABI_SIZE NAMES arm-none-eabi-size HINTS "${ARM_GCC_BIN_DIR}")

if(NOT ARM_NONE_EABI_GXX OR NOT ARM_NONE_EABI_OBJCOPY OR NOT ARM_NONE_EABI_SIZE)
	message(FATAL_ERROR
		"Incomplete GNU Arm Embedded Toolchain in '${ARM_GCC_BIN_DIR}'. "
		"Expected: arm-none-eabi-g++, arm-none-eabi-objcopy, arm-none-eabi-size")
endif()

set(CMAKE_C_COMPILER                "${ARM_NONE_EABI_GCC}")
set(CMAKE_ASM_COMPILER              "${ARM_NONE_EABI_GCC}")
set(CMAKE_CXX_COMPILER              "${ARM_NONE_EABI_GXX}")
set(CMAKE_LINKER                    "${ARM_NONE_EABI_GXX}")
set(CMAKE_OBJCOPY                   "${ARM_NONE_EABI_OBJCOPY}")
set(CMAKE_SIZE                      "${ARM_NONE_EABI_SIZE}")

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU specific flags
set(TARGET_FLAGS "-mcpu=cortex-m0plus ")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp -MMD -MP")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fdata-sections -ffunction-sections")

set(CMAKE_C_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_C_FLAGS_RELEASE "-Os -g0")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -g0")

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions -fno-threadsafe-statics")

set(CMAKE_EXE_LINKER_FLAGS "${TARGET_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T \"${CMAKE_SOURCE_DIR}/STM32G030XX_FLASH.ld\"")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --specs=nano.specs")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map=${CMAKE_PROJECT_NAME}.map -Wl,--gc-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--print-memory-usage")
set(TOOLCHAIN_LINK_LIBRARIES "m")

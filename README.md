# OpenSX70-Core

This repository contains the OpenSX70 STM32 firmware project.

## Compile Requirements

To build this project, install the following tools:

1. CMake
2. Ninja build
3. GNU Arm Embedded Toolchain

Currently the compile size for the debug build is too large for the SKU of STM32G0 we are using. Use the release preset until we move to a SKU with more flash.
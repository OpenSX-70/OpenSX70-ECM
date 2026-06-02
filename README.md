# OpenSX70-ECM

This repository contains the OpenSX70 ECM STM32 firmware project.

## Compile Requirements

To build this project, install the following tools:

1. CMake
2. Ninja build
3. GNU Arm Embedded Toolchain

### Windows

```powershell
winget install --id Kitware.CMake -e
winget install --id Ninja-build.Ninja -e
winget install --id Arm.GnuArmEmbeddedToolchain -e
```

### macOS

Install with Homebrew:

```bash
brew update
brew install cmake ninja arm-none-eabi-gcc
```

### Debian/Ubuntu Linux

Install with `apt`:

```bash
sudo apt update
sudo apt install -y cmake ninja-build gcc-arm-none-eabi binutils-arm-none-eabi
```

### Arch Linux

Install with `pacman`:

```bash
sudo pacman -Syu --needed cmake ninja arm-none-eabi-gcc arm-none-eabi-binutils
```


Currently the compile size for the debug build is too large for the SKU of STM32G0 we are using. Use the release preset until we move to a SKU with more flash.

## Updating your ECM

The following section is for installed ECMs. A guide for updating the board with the pogo headers rather than the FPC to SWD interface will be added at a later date, pending finding a good source for the pogo connector/designing our own.

This method required your camera to be on and powered.

1. Insert the 8 pin FPC cable in your ECM
2. Plug the FPC cable into the SWD interface
3. Plug STlinkv3 minie into SWD interface

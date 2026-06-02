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

### Debian based

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

## Compiling

Once prereqs are installed, run `cmake --build  --preset Release` in the OpenSX70_ECM folder. The resulting binary (elf) file will be under OpenSX70_ECM/build/Release


Currently the compile size for the debug build is too large for the SKU of STM32G0 we are using. Use the release preset until we move to a SKU with more flash.

## Updating your ECM

### Requirements

1. Stm32cubeProgrammer, requires ST microelectronics account [Link](https://www.st.com/en/development-tools/stm32cubeprog.html)
2. FPC to SWD interface (Will be uploaded soon)
3. 8 pin .5mm pitch FPC cable [Link](https://www.mouser.com/ProductDetail/538-15166-0080)
4. STlink v3 minie [Link](https://www.mouser.com/ProductDetail/STMicroelectronics/STLINK-V3MINIE)
5. (apple silicon only) Any USB hub. The v3 minie does not play nicely otherwise.

### Updating via FPC to SWD interface

The following section is for installed ECMs. A guide for updating the board with the pogo headers rather than the FPC to SWD interface will be added at a later date, pending finding a good source for the pogo connector/designing our own.

This method required your camera to be on and powered.

1. Insert the 8 pin FPC cable in your ECM
2. Insert the FPC cable into the SWD interface
3. Connect the STlink V3 minie into SWD interface
4. Plug your STlink into your computer
5. Open Stm32CubeProgrammer and hit `connect`<img width="1202" height="746" alt="image" src="https://github.com/user-attachments/assets/9d27b96e-6bef-4cbb-8b6c-19a5be9d7ea6" />
6. Click `Erasing and Programming`<img width="1202" height="746" alt="image" src="https://github.com/user-attachments/assets/80d623be-1232-4443-82d4-94e9d391f47f" />
7. Find the binary you wish to upload to your ECM by hitting `browse`. Select `Verify Programming` and `Run after Programming`<img width="1202" height="746" alt="image" src="https://github.com/user-attachments/assets/d4752fc8-0c49-4622-9b9d-3c975781d604" />
8. Hit `Start Programming`<img width="1202" height="746" alt="image" src="https://github.com/user-attachments/assets/39bc90de-ccb6-460a-9dd9-04c2f45f53a5" />





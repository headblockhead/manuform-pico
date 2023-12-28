# SQUIRREL Pico: SQUIRREL for the Raspberry Pi Pico.
<sup>or: Simplified QMK Uniquely Immaculate (and) Readable Runtime Editable Library</sup>

🚧 This project is currently **under construction**, so do not expect a usable result yet! 🚧

This is an implementation of [SQUIRREL](https://github.com/headblockhead/SQUIRREL) as a template example for a 5-key keyboard.

## Guide

 - [external/](external/) Exernal libraries, maintained seperatly to the project.
     - [pico-sdk](external/pico-sdk/) The pico SDK, used to build for the pico.
     - [squirrel](external/squirrel/) The SQUIRREL library, provides key functions and quantum layers, etc.
 - [src/](src/) Source code of the project.
     - [main.c](src/main.c) Main file, runs input checking and GPIOs.
     - [tinyusb/](src/tinyusb/) Stores tinyusb config files.
         - [tusb_config.h](src/tinyusb/tusb_config.h) Defines common configuration and device classes.
         - [usb_descriptors.c](src/tinyusb/usb_descriptors.c) Defines manufacturer and product ID and other HID constants.
         - [usb_descriptors.h](src/tinyusb/usb_descriptors.h) Defines HID report IDs.
 - [.gitignore](.gitignore) Removes some files from version control to not track build results, etc.
 - [.gitmodules](.gitmodules) Defines the submodules (in [external](external/))
 - [CMakeLists.txt](CMakeLists.txt) Defines how to build the project and its libraries.
 - [README.md](README.md) This file (documentation and [XC](https://github.com/joerdav/xc) tasks.)
 - [shell.nix](shell.nix) A [nix](https://nixos.org/) shell file that provides a reproducable and declarative environment for building and debugging.


## Tasks

### Build
Directory: ./build

Builds the keyboard firmware.

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4 SQUIRREL-pico
```

### Build-debug
Directory: ./build

Builds the keyboard firmware with debug build type.

```bash
cmake -DCMAKE_BUILD_TYPE=Debug .. 
make -j4 SQUIRREL-pico
cp compile_commands.json ../ # Copies the autocomplete information for ccls.
```

### Clean
Cleans the build directory for a fresh build.

```bash
rm -rf ./build
mkdir build
```

### Init-submodules

Fetches SQUIRREL and the pico-sdk submodules for use in the project.
```bash
git submodule update --init --recursive
```

### Update-submodules
Requires: init-submodules

This fetches the latest version of SQUIRREL and the pico-sdk from their remotes. Be careful of breaking changes!

```bash
git submodule update --remote
```

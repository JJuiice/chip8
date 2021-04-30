# Chip 8 Emulator

Chip-8 emulator written in C

## Requirements
- CMake

### Windows
- MinGW
### UNIX / OS X
- GCC or Clang
- libsdl2 and libsdl2-dev
- build-essentials

## Build
```
git clone https://github.com/JJuiice/chip8.git
cd chip8
mkdir build
cd build
cmake [-G "MinGW Makefiles"] ..
make
``` 

## Usage
- Run the `chip8` binary file via the command-line with the filepath to a Chip-8 ROM as a single argument
- Drag-&-Drop ROM onto `chip8` binary

## References
- [SDL Wiki](https://wiki.libsdl.org/)
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

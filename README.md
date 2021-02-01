# Chip 8 Emulator (WiP)

## Requirements
- gcc
- mingw-w64 (for Windows)
- libsdl2-dev

- Development was on Windows 10 machine with WSL (Ubuntu) as dev environment and both lnx and win64 SDL2 headers downloaded

## Usage
```
git clone https://github.com/O-Juice/chip8.git
cd chip8
make [win (default, windows [.exe]) | lnx (Linux [.out])]
``` 
Resulting binary will be an `emulator` binary file

## References
- [SDL Wiki](https://wiki.libsdl.org/)
- [SDL Source Code](https://hg.libsdl.org/)
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- https://www.khronos.org/opengl/wiki/Main_Page

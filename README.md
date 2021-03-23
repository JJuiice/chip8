# Chip 8 Emulator

Chip-8 emulator written in C

## Requirements
- gcc (Linux)
- mingw-w64 (Windows)
- OS-specific SDL2 development and runtime headers
- Python (optional)

## Build
```
git clone https://github.com/O-Juice/chip8.git
cd chip8
``` 
__Windows:__ `make`  
__Linux:__ `make lnx`  
__Emscripten:__ `make emcc`  
__Clean:__ `make clean`

In order to run the emscripten version, environment variable TARGET_ROM must be set to a *.ch8 within the same directory as the executable and emsdk environment should be active. Use the following command for the appropriate version of python installed on your system and navigate to `localhost:8000`:  

Python 2 - `python -m SimpleHTTPServer`  
Python 3 - `python -m http.server`  

__NOTICE:__
The esmcripten version of the game runs much slower than the desktop applications

## Usage
- Run the `emulator` binary file via the command-line with the filepath to a Chip-8 ROM as a single argument
- Drag & Drop ROM on to `emulator` binary

### __Note__
- Windows SDL2 *include* and *lib* directory path are read from the __WIN_MINGW_INC__ and __WIN_MINGW_LIBS__ environment variables in the Makefile

## References
- [SDL Wiki](https://wiki.libsdl.org/)
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

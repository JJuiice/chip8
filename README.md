# Chip 8 Emulator

Chip-8 emulator written in C

## Requirements
- gcc (Linux)
- mingw-w64 (Windows)
- OS-specific SDL2 development and runtime headers

## Build
```
git clone https://github.com/O-Juice/chip8.git
cd chip8
``` 
__Windows:__  
`make`  
__Linux:__  
`make lnx`  
__Clean:__  
`make clean`

## Usage
- Run the `emulator` binary file via the command-line with the filepath to a Chip-8 ROM as a single argument
- Drag & Drop ROM on to `emulator` binary

### __Note__
- If MinGW was installed on Windows and not WSL (Ubuntu), the `CC` variable in the first line in the Makefile may need to be set to `gcc`
- Windows SDL2 *include* and *lib* directory path are read from the __WIN_MINGW_INC__ and __WIN_MINGW_LIBS__ environment variables in the Makefile

## References
- [SDL Wiki](https://wiki.libsdl.org/)
- http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

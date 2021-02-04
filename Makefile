CC=x86_64-w64-mingw32-gcc
INCLUDE_DIRS=-I.
WIN_INCLUDE_DIRS=$(INCLUDE_DIRS) -I$(WIN_MINGW_INC) 
LNX_INCLUDE_DIRS=$(INCLUDE_DIRS) -I/usr/include -I/usr/include/x86_64-linux-gnu
# O1-O3 and Og optimization messes with emulation
CFLAGS=-g -pedantic -std=c11
LFLAGS=-lm -lSDL2 -lSDL2main
WIN_EXE=emulator.exe
LNX_BIN=emulator.out
DEPS=chip8.h cpu.h gfx.h ins.h logging.h
OBJS=main.o cpu.o gfx.o ins.o logging.o

.PHONY: all clean win lnx

all: win

win: CFLAGS+=$(WIN_INCLUDE_DIRS)
win: LFLAGS+=-L$(WIN_MINGW_LIBS)
win: $(WIN_EXE) 

lnx: CC=gcc
lnx: CFLAGS+=$(LNX_INCLUDE_DIRS) -DLNX
lnx: $(LNX_BIN)

clean:
	rm -f $(OBJS) $(WIN_EXE) $(LNX_BIN)

$(WIN_EXE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

$(LNX_BIN): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

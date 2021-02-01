CC=x86_64-w64-mingw32-gcc
CFLAGS=-g -O2 -I.
LFLAGS=-lSDL2 -lSDL2main
EXE=emulator.exe
DEPS=chip8.h
OBJS=main.o chip8.o

all: $(EXE)

clean:
	rm $(OBJS) $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

%.o: %.c $(DEPS) 
	$(CC) -o -c $@ $^ $(CFLAGS)

#
# Copyright (C) 2020-2021 Ojas Anand
# 
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
#

CC=x86_64-w64-mingw32-gcc
INCLUDE_DIRS=-I.
WIN_ADDITIONAL_CFLAGS=-mwindows $(INCLUDE_DIRS) -I$(WIN_MINGW_INC) 
LNX_INCLUDE_DIRS=$(INCLUDE_DIRS) -I/usr/include -I/usr/include/x86_64-linux-gnu
# O1-O3 and Og optimization messes with emulation
CFLAGS=-g -pedantic -std=c11
LFLAGS=-lm -lSDL2 -lSDL2main
WIN_EXE=emulator.exe
LNX_BIN=emulator.out
DEPS=chip8.h cpu.h io.h ins.h logging.h
OBJS=main.o cpu.o io.o ins.o logging.o

.PHONY: all clean win lnx

all: win

win: CFLAGS+=$(WIN_ADDITIONAL_CFLAGS)
win: LFLAGS+=-L$(WIN_MINGW_LIBS)
win: $(WIN_EXE) 

lnx: CC=gcc
lnx: CFLAGS+=$(LNX_INCLUDE_DIRS)
lnx: $(LNX_BIN)

clean:
	rm -f $(OBJS) $(WIN_EXE) $(LNX_BIN)

$(WIN_EXE): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

$(LNX_BIN): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)
	chmod 755 $@

%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

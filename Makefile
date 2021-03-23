#
# Copyright (C) 2020-2021 Ojas Anand
# 
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# You should have received a copy of the GNU General Public License along with this program. If not, see https://www.gnu.org/licenses/. 
#

CC=x86_64-w64-mingw32-gcc
INCLUDE_DIRS=-I.
WIN_ADDITIONAL_CFLAGS=$(INCLUDE_DIRS) -I$(WIN_MINGW_INC) -mwindows -std=c11 
LNX_ADDITIONAL_CFLAGS=$(INCLUDE_DIRS) -I/usr/include -I/usr/include/x86_64-linux-gnu -std=gnu11
# O1-O3 and Og optimization messes with emulation
CFLAGS=-pedantic -Wall
LFLAGS=-lm -lSDL2 -lSDL2main
BUILD_DIR=build
EXE=emulator
WIN_EXE=$(EXE).exe
LNX_BIN=$(EXE).out
DEPS=cpu.h io.h ins.h logging.h
OBJS=main.o cpu.o io.o ins.o logging.o

ifdef OS
	RM = del /Q
else
	RM = rm -r
endif

.PHONY: all clean win lnx emcc

all: win

win: CFLAGS+=$(WIN_ADDITIONAL_CFLAGS)
win: LFLAGS+=-L$(WIN_MINGW_LIBS)
win: $(WIN_EXE) 

lnx: CC=gcc
lnx: CFLAGS+=$(LNX_INCLUDE_CFLAGS)
lnx: $(LNX_BIN)

emcc: | $(BUILD_DIR)
	@echo Module['arguments'] = ["$(TARGET_ROM)"]; > $(BUILD_DIR)/emsdkpre.js
	emcc main.c logging.c io.c ins.c cpu.c -o $(BUILD_DIR)/$(EXE).html -Wall -lm -s USE_SDL=2 --pre-js $(BUILD_DIR)/emsdkpre.js --preload-file "$(TARGET_ROM)" -I.

clean: | $(BUILD_DIR)
	$(RM) $(BUILD_DIR)

$(OBJS): %.o: %.c $(DEPS) | $(BUILD_DIR)
	$(CC) -c -o $(BUILD_DIR)/$@ $< $(CFLAGS)

$(WIN_EXE): $(OBJS) | $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CC) -o $@ $^ $(LFLAGS)

$(LNX_BIN): $(OBJS) | $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CC) -o $@ $^ $(LFLAGS) && chmod 755 $@

$(BUILD_DIR):
	mkdir $(BUILD_DIR) 

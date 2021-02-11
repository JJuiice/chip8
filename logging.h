#ifndef LOGGING_H
#define LOGGING_H
#include <stdio.h>

void logMsg(const char *msg);
void logSDLErrQuit(const char *msg);
void logErrQuit(const char *msg);
void checkSDLError(int line);
void dumpRegAndPointerInfo();

extern FILE *logFile;

#endif
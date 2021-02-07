#ifndef LOGGING_H
#define LOGGING_H

void logMsg(const char* msg);
void logQuit(const char* msg);
void checkSDLError(int line);
void dumpRegAndPointerInfo();

#endif
#ifndef LOGGING_H
#define LOGGING_H

void logMsg(const char*);
void logQuit(const char*);
void checkSDLError(int, const char*);
void dumpRegAndPointerInfo();

#endif
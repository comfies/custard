#ifndef CUSTARD_H
#define CUSTARD_H

#include "../vector.h"

extern char* rc_path;
extern unsigned short custard_is_running;

int custard(int, char**);
unsigned short initialize(void);
void finalize(void);

void _log(const char*, const char*, const int, char*, ...);
#define log(...) _log(__FILE__, __func__, __LINE__, __VA_ARGS__)
#define suppress_unused(...) (void)(__VA_ARGS__)

#endif

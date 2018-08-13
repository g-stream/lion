#ifndef li_util_h
#define li_util_h
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include "li_value.h"

#define cast(t, v) ((t*) (v))

uint32_t hashString(const char* s);

//compiler help functions
bool isAlphbeta(const char* c);
bool isLow(const char* c);
bool isUp(const char* c);
bool isNum(const char* c);
bool isCharOfVar(const char* c);
bool isHex(const char* c);
int hexToNum(const char* c);
bool isOneOfChars(const char* c, const char* pt);
bool isSkip(const char* c);
#endif

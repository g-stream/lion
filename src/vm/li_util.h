#ifndef li_util_h
#define li_util_h
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
uint32_t hash_str(const char* s);

//compiler help functions
bool isAlphbeta(const char* c);
bool isLow(const char* c);
bool isUp(const char* c);
bool isNum(const char* c);
bool isCharOfVarStart(const char* c);
bool isHex(const char* c);
int hexToNum(const char* c);
bool isOneOfChars(const char* c, const char* pt);

#endif

#include "li_util.h"
#include "li_value.h"
#include "li_mem.h"
#define HASH_VAL_A 54059 /* a prime */
#define HASH_VAL_B 76963 /* another prime */
#define HASH_VAL_C 86969 /* yet another prime */
#define HASH_VAL_FIRSTH 37 /* also prime */
unsigned hashString(const char* s)
{
   unsigned h = HASH_VAL_FIRSTH;
   while (*s) {
     h = (h * HASH_VAL_A) ^ (s[0] * HASH_VAL_B);
     s++;
   }
   return h; // or return h % C;
}

/* compiler help functions
 */

bool isAlphbeta(const char* c){
    if(('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z'))
        return true;
    return false;
}
bool isLow(const char* c){
    if('a' <= *c && *c <= 'z')
        return true;
    return false;
}
bool isUp(const char* c){
    if('A' <= *c && *c <= 'Z')
        return true;
    return false;
}
bool isNum(const char* c){
    if('0' <= *c && *c <= '9')
        return true;
    return false;
}
bool isCharOfVar(const char* c){
    if(*c == '_' || isAlphbeta(c) || isNum(c))
        return true;
    return false;
}
bool isHex(const char* c){
    if(('A' <= *c && *c <= 'F') || ('a' <= *c && *c <= 'F') || isNum(c))
        return true;
    return false;
}
int hexToNum(const char* c){
    assert(isAlphbeta(c) || isNum(c));
    if(isNum(c)){
        return *c - '0';
    } 
    else if(isUp(c)){
        return *c - 'A' + 10;
    }
    else{
        return *c - 'a' + 10;
    }
}

bool isOneOfChars(const char* c, const char* pt){
    while(*pt != '\0'){
        if(*c == *pt)
            return true;
        pt++;
    }
    return false;
}

bool isSkip(const char* c){
    return isOneOfChars(c, " \n\r\t");
}

DEFINE_BUFFER(Byte, uint8_t);
DEFINE_BUFFER(Int, int);
DEFINE_BUFFER(String, ObjString*);

void liSymbolTableInit(SymbolTable* symbols)
{
    liStringBufferInit(symbols);
}

void liSymbolTableClear(LionVm* vm, SymbolTable* symbols)
{
    liStringBufferClear(vm, symbols);
}

int liSymbolTableAdd(LionVm* vm, SymbolTable* symbols,
                       const char* name, size_t length)
{
  ObjString* symbol = valueToString(liNewString(vm, name));
  
  liStringBufferWrite(vm, symbols, symbol);
  
  return symbols->count - 1;
}

int liSymbolTableEnsure(LionVm* vm, SymbolTable* symbols,
                          const char* name, size_t length)
{
  // See if the symbol is already defined.
  int existing = liSymbolTableFind(symbols, name, length);
  if (existing != -1) return existing;

  // New symbol, so add it.
  return liSymbolTableAdd(vm, symbols, name, length);
}

int liSymbolTableFind(const SymbolTable* symbols,
                        const char* name, size_t length)
{
  // See if the symbol is already defined.
  // TODO: O(n). Do something better.
  for (int i = 0; i < symbols->count; i++)
  {
    if (strcpy(symbols->data[i]->content, name) == 0) return i;
  }
  return -1;
}


// From: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float
int liPowerOf2Ceil(int n)
{
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  
  return n;
}

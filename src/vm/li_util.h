#ifndef li_util_h
#define li_util_h
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
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




typedef struct sObjString ObjString;
typedef struct sLionVm LionVm;

#define DECLARE_BUFFER(name, type) \
    typedef struct \
    { \
      type* data; \
      int count; \
      int capacity; \
    } name##Buffer; \
    void li##name##BufferInit(name##Buffer* buffer); \
    void li##name##BufferClear(LionVm* vm, name##Buffer* buffer); \
    void li##name##BufferFill(LionVm* vm, name##Buffer* buffer, type data, \
                                int count); \
    void li##name##BufferWrite(LionVm* vm, name##Buffer* buffer, type data)

// This should be used once for each type instantiation, somewhere in a .c file.
#define DEFINE_BUFFER(name, type) \
    void li##name##BufferInit(name##Buffer* buffer) \
    { \
      buffer->data = NULL; \
      buffer->capacity = 0; \
      buffer->count = 0; \
    } \
    \
    void li##name##BufferClear(LionVm* vm, name##Buffer* buffer) \
    { \
      liM_realloc_(vm, buffer->data, 0, 0); \
      li##name##BufferInit(buffer); \
    } \
    \
    void li##name##BufferFill(LionVm* vm, name##Buffer* buffer, type data, \
                                int count) \
    { \
      if (buffer->capacity < buffer->count + count) \
      { \
        int capacity = liPowerOf2Ceil(buffer->count + count); \
        buffer->data = (type*)liM_realloc_(vm, buffer->data, \
            buffer->capacity * sizeof(type), capacity * sizeof(type)); \
        buffer->capacity = capacity; \
      } \
      \
      for (int i = 0; i < count; i++) \
      { \
        buffer->data[buffer->count++] = data; \
      } \
    } \
    \
    void li##name##BufferWrite(LionVm* vm, name##Buffer* buffer, type data) \
    { \
      li##name##BufferFill(vm, buffer, data, 1); \
    }

DECLARE_BUFFER(Byte, uint8_t);
DECLARE_BUFFER(Int, int);
DECLARE_BUFFER(String, ObjString*);


typedef StringBuffer SymbolTable;

// Initializes the symbol table.
void liSymbolTableInit(SymbolTable* symbols);

// Frees all dynamically allocated memory used by the symbol table, but not the
// SymbolTable itself.
void liSymbolTableClear(LionVm* vm, SymbolTable* symbols);

// Adds name to the symbol table. Returns the index of it in the table.
int liSymbolTableAdd(LionVm* vm, SymbolTable* symbols,
                       const char* name, size_t length);

// Adds name to the symbol table. Returns the index of it in the table. Will
// use an existing symbol if already present.
int liSymbolTableEnsure(LionVm* vm, SymbolTable* symbols,
                          const char* name, size_t length);

// Looks up name in the symbol table. Returns its index if found or -1 if not.
int liSymbolTableFind(const SymbolTable* symbols,
                        const char* name, size_t length);



int liPowerOf2Ceil(int n);

#endif

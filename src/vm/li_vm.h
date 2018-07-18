#ifndef li_vm_h
#define li_vm_h
#include "li_value.h"
typedef struct sCallInfo {
    
} CallInfo;

typedef struct sGlobalState {
    CallInfo* callinfo;
} GlobalState;


void run_vm(const char* src);
void run_repl();

typedef struct sLionState {
  CommonHeader;
  
} LionState;

#endif

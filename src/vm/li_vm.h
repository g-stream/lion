#ifndef li_vm_h
#define li_vm_h
typedef struct sGlobalState GlobalState;
typedef struct sCallInfo CallInfo;
typedef struct 


typedef struct sGlobalState {
    CallInfo* callinfo
} GlobalState



typedef struct sCallInfo {
    UpValue upvalues;
    
} CallInfo
    
#endif

#include "li_vm.h"
#include "li_config.h"
#include "li_value.h"
#include "li_mem.h"
#include "li_vm_macro.h"
#include "li_debug.h"
ObjFiber* liNewFiber(){
    ObjFiber* fiber = (ObjFiber*) malloc(sizeof(ObjFiber));
    return fiber;
}
static void liInitFiber(LionVm* vm, ObjFiber* fiber){
    fiber->stack = liNewArrayOfType(vm, Value, LION_BASIC_STACK_SIZE);
    fiber->stack_capacity = LION_BASIC_STACK_SIZE;
    fiber->stack_top = fiber->stack;
}



void run_vm(const char* src){
    LionVm* vm = liNewVm();
    memset(vm, 0, sizeof(LionVm));
    vm->fiber = (ObjFiber*) malloc(sizeof(ObjFiber));
    memset(vm->fiber, 0, sizeof(ObjFiber));
}



void run_repl(){
    
}

#define VM_LOOP() LOOP_START: \
    instrcution = *(ip++); \
    switch(GET_OPCODE(instrcution))
#define CASE(l)	case l:
#define DISPATCH()  goto LOOP_START

extern const char* op_name[];



void liExecute(LionVm* vm, ObjFiber* fiber){
    vm->fiber = fiber;
    register CallFrame* frame;
    register stkid_t stack_start;
    register instruction_t* ip;
    register instruction_t instrcution;
    register ObjFn* fn;
    
    vm->fiber = fiber;
    frame = fiber->base_frame + fiber->num_frames - 1;
    stack_start = frame->stack_start;
    fn = frame->closure->fn;
    ip = fn->code;
    
    VM_LOOP(){
        CASE(OP_ADD){
            
            
        }DISPATCH();
    }
}
    
    

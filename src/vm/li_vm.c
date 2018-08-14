#include "li_vm.h"

void run_vm(const char* src){
    LionVm* vm = newVm();
    memset(vm, 0, sizeof(LionVm));
    vm->fiber = (ObjFiber*) malloc(sizeof(ObjFiber));
    memset(vm->fiber, 0, sizeof(ObjFiber));
    
}
void run_repl(){
}

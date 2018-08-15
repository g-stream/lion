#ifndef li_men_h
#define li_men_h
#include <stdlib.h>
#include "li_value.h"
#include "li_util.h"

#define liMalloc(vm, size) liM_realloc_(vm, NULL, 0, (size))
#define liFree(vm, block) liM_realloc_(vm, block, sizeof(*(block)), 0)
#define liNewValueOfType(vm, type) cast(type *, liMalloc(vm, sizeof(type)))
#define liNewArrayOfType(vm, type, size) cast(type *, liMalloc(vm, size*sizeof(type)))

void* liM_realloc_(LionVm *vm, void *block, size_t osize, size_t nsize);


#endif

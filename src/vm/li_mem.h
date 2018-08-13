#ifndef li_men_h
#define li_men_h
#include <stdlib.h>
#include "li_value.h"
#include "li_util.h"

#define liMalloc(fiber, size) liM_realloc_(fiber, NULL, 0, (size))
#define liFree(fiber, block) liM_realloc_(fiber, block, sizeof(*(block)), 0)
#define liNewValueOfType(fiber, type) cast(type, liMalloc(fiber, sizeof(type)))
#define liNewArrayOfType(fiber, type, size) cast(type, liMalloc(fiber, size*sizeof(type)))

void* liM_realloc_(ObjFiber *fiber, void *block, size_t osize, size_t nsize);


#endif

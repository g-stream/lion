#include "li_mem.h"
#include <stdlib.h>
#include "li_debug.h"
void* liM_realloc_(LionVm *vm, void *block, size_t osize, size_t nsize) {
    size_t realosize = (block) ? osize : 0;
    ASSERT((realosize == 0) == (block == NULL), "no empty pointer must have size or empty pointer must not have size");
    if(nsize == 0){
        ASSERT(block, "if new size is not zero, pointer must not been empty");
        free(block);
        return NULL;
    }
    if(realosize == 0){
        return malloc(nsize);
    }
    return realloc(block, nsize);
}

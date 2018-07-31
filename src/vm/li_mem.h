#ifndef li_men_h
#define li_men_h
#include <stdlib.h>
#include "li_value.h"
#include "li_config.h"
#include "li_vm.h"


void liRealloc();

void li_mem_alloc();
void li_mem_free();
void li_mem_realloc();

#endif

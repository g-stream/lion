#ifndef li_men_h
#define li_men_h
#include <stdlib.h>
void liRealloc(void** p, size_t old_size, size_t new_size){
    if(old_size == 0){
        *p = malloc(new_size);
    } 
    else if(new_size == 0){
        *p = realloc(p, new_size);
    } else {
        free(*p);
        *p = NULL;
    }
}

#endif

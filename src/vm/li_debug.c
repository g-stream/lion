#include "li_debug.h"
#include "li_compiler.h"
#include <stdio.h>

#define ERROR(err) #err,
const char* li_error_names[] = {
li_errors 
};
#undef ERROR 

void printWithHighlight(const char* str, size_t pos) {
    printf("%s\n", str);
    for(int i = 0; i < pos; ++i)
        putchar(' ');
    printf("^\n");
}

void printError( ErrorType err_type, const char* info){
    printf(li_error_names[err_type]);
    printf(" : ");
    printf(info);
    putchar('\n');
}

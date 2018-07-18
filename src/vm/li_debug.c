#include "li_debug.h"
#include <stdio.h>

void print_with_highlight(const char* str, size_t pos) {
    printf("%s\n", str);
    for(int i = 0; i < pos; ++i)
        putchar(' ');
    printf("^\n");
}

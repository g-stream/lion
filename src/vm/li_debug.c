<<<<<<< HEAD
#include "li_debug.h"
#include <stdio.h>

void print_with_highlight(const char* str, size_t pos) {
    printf("%s\n", str);
    for(int i = 0; i < pos; ++i)
        putchar(' ');
    printf("^\n");
}
=======

>>>>>>> 8ae7a26a57297e527c71cdfaa0fbb7c482ea9b11

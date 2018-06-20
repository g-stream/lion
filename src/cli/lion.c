#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LION_VERSION "0.1"
int main(int argc, char** argv){
    if(argc == 2 && strcmp(argv[1], "--help") == 0){
        printf("Usage: lion [file] [arguments...]\n");
        printf("  --help Show usage\n");
        return 0;
    }
    if(argc == 2 && strcmp(argv[1], "--version") == 0){
        printf("lion: %s\tby yinly\n", LION_VERSION);
        return 0;
    }
    if(argc == 1){
        char str[100];
        while(1){
            fgets(str, 100, stdin);
            printf("you input: %s\n", str);
        }
    }
    return 0;
}

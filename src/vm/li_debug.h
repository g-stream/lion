#ifndef li_debug_h
#define li_debug_h
#include <stdio.h>
#include <assert.h>
/* 
 * define errors 
 */
#define li_errors ERROR(CompilerError)\
                  ERROR(RuntimeError)\
                  ERROR(ParsingError)
                  
#define ERROR(err) err,
typedef enum {
li_errors
} ErrorType;
#undef ERROR 
void printWithHighlight(const char* str, size_t pos);
void printError(ErrorType err_type, const char* info);
#endif

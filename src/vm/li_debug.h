#ifndef li_debug_h
#define li_debug_h
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "li_config.h"
/* 
 * define errors 
 */
#define li_errors ERROR(CompilerError)\
                  ERROR(RuntimeError)\
                  ERROR(ParsingError)\
                  ERROR(SyntaxError)
                  
#define ERROR(err) err,
typedef enum {
li_errors
} ErrorType;
#undef ERROR 
void printWithHighlight(const char* str, size_t pos);
void printError(ErrorType err_type, const char* info);

#if LION_DEBUG
#define ASSERT(condition, message)	do { \
												if (!(condition)) { \
													fprintf(stderr, "[%s:%d] Assert failed in %s(): %s\n", \
													__FILE__, __LINE__, __func__, message); \
													abort(); \
												} \
											} \
											while(0)
#else
#define DEBUG_ASSERT(condition, message)
#endif

#endif

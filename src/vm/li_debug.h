#ifndef li_debug_h
#define li_debug_h
#include <stdio.h>
#include <assert.h>
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

#ifdef LION_DEBUG
  #include <stdio.h>
  #define ASSERT(condition, message) \
      do \
      { \
        if (!(condition)) \
        { \
          fprintf(stderr, "[%s:%d] Assert failed in %s(): %s\n", \
              __FILE__, __LINE__, __func__, message); \
          abort(); \
        } \
      } \
      while(0)

  // Indicates that we know execution should never reach this point in the
  // program. In debug mode, we assert this fact because it's a bug to get here.
  //
  // In release mode, we use compiler-specific built in functions to tell the
  // compiler the code can't be reached. This avoids "missing return" warnings
  // in some cases and also lets it perform some optimizations by assuming the
  // code is never reached.
  #define UNREACHABLE() \
      do \
      { \
        fprintf(stderr, "[%s:%d] This code should not be reached in %s()\n", \
            __FILE__, __LINE__, __func__); \
        abort(); \
      } \
      while (0)

#else
    #define ASSERT(condition, message)
    #define UNREACHABLE() 
#endif
          
#endif

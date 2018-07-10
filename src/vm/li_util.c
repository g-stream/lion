#include "li_util.h"

#define HASH_VAL_A 54059 /* a prime */
#define HASH_VAL_B 76963 /* another prime */
#define HASH_VAL_C 86969 /* yet another prime */
#define HASH_VAL_FIRSTH 37 /* also prime */
unsigned hash_str(const char* s)
{
   unsigned h = HASH_VAL_FIRSTH;
   while (*s) {
     h = (h * HASH_VAL_A) ^ (s[0] * HASH_VAL_B);
     s++;
   }
   return h; // or return h % C;
}

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

/* compiler help functions
 */

bool isAlphbeta(const char* c){
    if(('a' <= *c && *c <= 'z') || ('A' <= *c && *c <= 'Z'))
        return true;
    return false;
}
bool isLow(const char* c){
    if('a' <= *c && *c <= 'z')
        return true;
    return false;
}
bool isUp(const char* c){
    if('A' <= *c && *c <= 'Z')
        return true;
    return false;
}
bool isNum(const char* c){
    if('0' <= *c && *c <= '9')
        return true;
    return false;
}
bool isCharOfVar(const char* c){
    if(*c == '_' || isAlphbeta(c) || isNum(c))
        return true;
    return false;
}
bool isHex(const char* c){
    if(('A' <= *c && *c <= 'F') || ('a' <= *c && *c <= 'F') || isNum(c))
        return true;
    return false;
}
int hexToNum(const char* c){
    assert(isAlphbeta(c) || isNum(c));
    if(isNum(c)){
        return *c - '0';
    } 
    else if(isUp(c)){
        return *c - 'A' + 10;
    }
    else{
        return *c - 'a' + 10;
    }
}

bool isOneOfChars(const char* c, const char* pt){
    while(*pt != '\0'){
        if(*c == *pt)
            return true;
        pt++;
    }
    return false;
}

bool isSkip(const char* c){
    return isOneOfChars(c, " \n\r\t");
}

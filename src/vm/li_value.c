#include "li_value.h"
void value2string(Value v){
    switch(v){
        case FALSE_VAL:
            printf("false\n");
            break;
        case TRUE_VAL:
            printf("true\n");
            break;
        case NULL_VAL:
            printf("null\n");
            break;
        case UNDEF_VAL:
            printf("undefined\n");
            break;
        default:
            if(IS_NUM(v)){
                printf("%f\n", (double)v);
            }else{
                printf("haven't implemented\n");
            }
    }
}


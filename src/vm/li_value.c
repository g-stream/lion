#include "li_value.h"
#include "li_debug.h"
#include "math.h"
void value2string(Value v){
    switch(v){
        case FALSE_VAL:
            printf("false\n");
            break;
        case TRUE_VAL:
            printf("true\n");
            break;
        case NIL_VAL:
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

Value numNeg(Value v){
    ValueBit vb;
    ASSERT(IS_NUM(v), "should be passed a number value");
    vb.value = v;
    vb.asDouble = - vb.asDouble;
    return vb.value;
}
Value numAdd(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble += vb2.asDouble;
    return vb1.value;
}
Value numMinus(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble -= vb2.asDouble;
    return vb1.value;
}
Value numMulply(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble *= vb2.asDouble;
    return vb1.value;
}
Value numDiv(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble /= vb2.asDouble;
    return vb1.value;
}
Value numPow(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble = pow(vb1.asDouble, vb2.asDouble);
    return vb1.value;
}
Value numIDiv(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    vb1.asDouble += vb2.asDouble;
    modf(vb1.asDouble, &vb1.asDouble);
    return vb1.value;
}
Value numMod(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble;
    uint64_t v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double) v1%v2;
    return vb1.value;
}

Value bitNeg(Value v){
    ValueBit vb;
    ASSERT(IS_NUM(v) && modf(vb.asDouble, &vb.asDouble) == 0 , "should be passed an interger number value");
    vb.value = v;
    uint64_t tmp = (uint64_t) vb.asDouble;
    vb.asDouble = (double) (~ tmp);
    return vb.value;
}
Value bitAnd(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 & v2);
    return vb1.value;
}
Value bitOr(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 | v2);
    return vb1.value;
}
Value bitXor(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 ^ v2);
    return vb1.value;
}
Value bitShiftL(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 << v2);
    return vb1.value;
}
Value bitShiftR(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 >> v2);
    return vb1.value;
}

Value boolAdd(Value l, Value r)
Value boolOr(Value l, Value r);
Value boolXor(Value l, Value r);

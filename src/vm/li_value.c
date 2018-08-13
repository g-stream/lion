#include "li_value.h"
#include "li_debug.h"
#include "li_mem.h"
#include "li_util.h"
#include <string.h>

void printValue(Value v){
    
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
                ValueBit vb;
                vb.value = v;
                double intpart;
                if(modf(vb.asDouble, &intpart) == 0){
                    printf("%ld\n", (int64_t)intpart);
                }
                else{
                    printf("%f\n", vb.asDouble);
                }
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
    vb1.asDouble /= vb2.asDouble;
    modf(vb1.asDouble, &vb1.asDouble);
    return vb1.value;
}
Value numMod(Value l, Value r){
    ValueBit vb1, vb2;
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    vb1.value = l;vb2.value = r;
    int64_t v1 = (int64_t) vb1.asDouble;
    int64_t v2 = (int64_t) vb2.asDouble;
    vb1.asDouble = (double) (v1%v2);
    return vb1.value;
}

Value bitNeg(Value v){
    ValueBit vb;
    vb.value = v;
    ASSERT(IS_NUM(v) && modf(vb.asDouble, &vb.asDouble) == 0 , "should be passed an interger number value");
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

Value boolAnd(Value l, Value r){
    Value v1 = (IS_NULL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NULL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (IS_TRUE(v1) && IS_TRUE(v2))?TRUE_VAL:FALSE_VAL;
}
Value boolOr(Value l, Value r){
    Value v1 = (IS_NULL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NULL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (IS_TRUE(v1) || IS_TRUE(v2))?TRUE_VAL:FALSE_VAL;
}
Value boolXor(Value l, Value r){
    Value v1 = (IS_NULL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NULL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (v1 != v2)?TRUE_VAL:FALSE_VAL;
}

ObjString* stringWithLength(LionVm* vm, size_t len){
    ObjString* str = liNewValueOfType(vm->fiber, ObjString);
    str->content = liNewArrayOfType(vm->fiber, char, len);
    return str;
}

Value newString(LionVm* vm, const char* cstring){
    ObjString* str;
    if(cstring){
        str = stringWithLength(vm, strlen(cstring));
        memcpy(str->content, cstring, str->length);
        str->content[str->length] = '\0';
        str->hash = hashString(str->content);
    }
    else {
        str = stringWithLength(vm, 0);
        str->hash = 0;
    }
    return objStringToValue(str);
}
Value stringLength(LionVm* vm, ObjString* string){
    ValueBit vb;
    vb.asDouble = string->length;
    return vb.value;
}
Value stringSub(LionVm* vm, ObjString* string, size_t start, size_t end){
    //Todo start == end will meet some problem???
    size_t len = end - start;
    ASSERT(start <= end && end <= string->length, "index out of the length of string");
    ObjString* str = stringWithLength(vm, len);
    memcpy(str->content, string->content, len);
    return objStringToValue(str);
}
Value newUpvalue(LionVm* vm);



Value newClass(LionVm* vm);
Value classBindSuper(LionVm* vm, ObjClass* class, ObjClass* super_class);

Value newList(LionVm* vm);
Value listInsert(LionVm* vm, ObjList* list, Value value);
Value listRemove(LionVm* vm, ObjList* list, Value index);
Value listSize(LionVm* vm, ObjList* list);

Value newMap(LionVm* vm);
Value mapInsert(LionVm* vm, ObjMap* map, Value key, Value value);
Value mapRemove(LionVm* vm, ObjMap* map, Value key);
Value mapSize(LionVm* vm, ObjMap* map);


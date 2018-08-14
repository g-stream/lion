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
    ASSERT(IS_NUM(v), "should be passed a number value");
    return numToValue(- valueToNum(v));
}
Value numAdd(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) + valueToNum(r));
}
Value numMinus(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) - valueToNum(r));
}
Value numMulply(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) * valueToNum(r));
}
Value numDiv(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) / valueToNum(r));
}
Value numPow(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(pow(valueToNum(l), valueToNum(r)));
}
Value numIDiv(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    double res;
    modf(valueToNum(l) / valueToNum(r), &res);
    return numToValue(res);
}
Value numMod(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    int64_t v1 = (int64_t) valueToNum(l);
    int64_t v2 = (int64_t) valueToNum(r);
    return numToValue((double) (v1%v2));
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
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 & v2);
    return vb1.value;
}
Value bitOr(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 | v2);
    return vb1.value;
}
Value bitXor(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 ^ v2);
    return vb1.value;
}
Value bitShiftL(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    uint64_t v1 = (uint64_t) valueToNum(l), v2 = (uint64_t) valueToNum(r);
    return numToValue((double)(v1 << v2));
}
Value bitShiftR(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
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
    ObjString* str = liNewValueOfType(vm, ObjString);
    str->content = liNewArrayOfType(vm, char, len+1);
    return str;
}

Value newString(LionVm* vm, const char* cstring){
    ObjString* str;
    if(cstring){
        size_t length = strlen(cstring);
        str = stringWithLength(vm, length);
        str->length = length;
        memcpy(str->content, cstring, length);
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
void  printString(Value v){
    printf("%s\n", valueToString(v)->content);
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


LionVm* newVm(){
    LionVm* vm = (LionVm*) malloc(sizeof(LionVm));
    
    return vm;
}


#ifndef li_value_h
#define li_value_h
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
typedef enum {
  OBJ_CLASS,
  OBJ_CLOSURE,
  OBJ_FIBER,
  OBJ_FN,
  OBJ_FOREIGN,
  OBJ_INSTANCE,
  OBJ_LIST,
  OBJ_MAP,
  OBJ_MODULE,
  OBJ_UPVALUE
} ObjType;


typedef uint64_t Value;
#define SIGN_BIT ((uint64_t)1 << 63)
#define QNAN ((uint64_t)0X7ff0000000000000)
#define OBJPOINTER_MASK  ((((uint64_t)1)<<48) -1)

// Tag values for the different singleton values.
/*
 * for double type, if the 52nd to 63rd bits is setted, it is a nan value. So we use 
 * other bits of a nan value to reperesent our different type.
 * Use the three bits before the bits setted for nan to reperesent singleton value types below
 * If the top most bit is setted, the value is a class, the lowest 32 bit is used to reperesent the pointer to the value
 */
#define TAG_NAN         ((uint64_t)0 << 48)
#define TAG_NIL         ((uint64_t)1 << 48)
#define TAG_FALSE       ((uint64_t)2 << 48)
#define TAG_TRUE        ((uint64_t)3 << 48)
#define TAG_UNDEF       ((uint64_t)4 << 48)
#define TAG_STRING      ((uint64_t)5 << 48)
#define TAG_CLASS       ((uint64_t)6 << 48)
#define TAG_CLOSURE     ((uint64_t)7 << 48)
#define TAG_FIBER       ((uint64_t)8 << 48)
#define TAG_FN          ((uint64_t)9 << 48)
#define TAG_FOREIGH     ((uint64_t)10 << 48)
#define TAG_INSTANCE    ((uint64_t)11 << 48)
#define TAG_LIST        ((uint64_t)12 << 48)
#define TAG_MAP         ((uint64_t)13 << 48)

// Masks out the tag bits used to identify the singleton value.
#define MASK_TAG      ((uint64_t)15 << 48)

// Singleton values.
#define NIL_VAL      ((Value)(uint64_t)(QNAN | TAG_NIL))
#define FALSE_VAL     ((Value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL      ((Value)(uint64_t)(QNAN | TAG_TRUE))
#define UNDEF_VAL     ((Value)(uint64_t)(QNAN | TAG_UNDEF))

#define IS_NUM(value) (((value) & QNAN) != QNAN)
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_FALSE(value)     ((value) == FALSE_VAL)
#define IS_TRUE(value)      ((value) == TRUE_VAL)
#define IS_BOOLEAN(value)   (IS_FALSE(value) || IS_TRUE(value))
#define IS_NULL(value)      ((value) == NIL_VAL)
#define IS_UNDEF(value)     ((value) == UNDEFINED_VAL)
#define IS_STR(value)       (((value)&(QNAN | MASK_TAG | SIGN_BIT)) == (QNAN | TAG_STR))




// Value -> 0 or 1.
#define AS_BOOL(value) ((value) == TRUE_VAL)

// Value -> Obj*.
#define AS_OBJ(value) ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))
#define AS_STR_INDEX(value) ((uint32_t)((value) & ~(SIGN_BIT|QNAN|MASK_TAG)))



typedef union {
    Value    value;
    double   asDouble;
    uint64_t asUint64;
    uint32_t as2Uint32[2];
} ValueBit;



typedef enum{
    VAL_NULL,
    VAL_FALSE,
    VAL_TRUE,
    VAL_NUM,
    VAL_STR,
    VAL_UNDEF,
    VAL_OBJ
} ValueType;

void printValue(Value v);

Value numNeg(Value v);
Value numAdd(Value l, Value r);
Value numMinus(Value l, Value r);
Value numMulply(Value l, Value r);
Value numDiv(Value l, Value r);
Value numPow(Value l, Value r);
Value numIDiv(Value l, Value r);
Value numMod(Value l, Value r);

Value bitNeg(Value v);
Value bitAnd(Value l, Value r);
Value bitOr(Value l, Value r);
Value bitXor(Value l, Value r);
Value bitShiftL(Value l, Value r);
Value bitShiftR(Value l, Value r);

Value boolAnd(Value l, Value r);
Value boolOr(Value l, Value r);
Value boolXor(Value l, Value r);



typedef struct sCallInfo {
    
} CallInfo;



typedef struct sObjString{
    uint32_t length;
    uint32_t hash;
    char* content;
} ObjString;

typedef struct sObjUpvalue{
    bool   closed;
    Value* value;
    struct sObjUpvalue* next;
}ObjUpvalue;

typedef struct sObjFn{
    
}ObjFn;

typedef struct sObjClass{
    struct sObjClass* super_class;
    
}ObjClass;

typedef struct sObjClosure{
    
}ObjClosure;

typedef struct sObjFiber{
    
}ObjFiber;

typedef struct sObjForeign{
    
}ObjForeign;

typedef struct sObjInstance{
    
}ObjInstance;

typedef struct sOjbList{
    size_t n;
    
}ObjList;

typedef struct sObjMap{
    
}ObjMap;

typedef struct sObjModule{
    
}ObjModule;





typedef struct sLionVm {
    
    CallInfo* callinfo;
    ObjFiber* fiber;
} LionVm;

LionVm* newVm();

Value newString(LionVm* vm, const char* cstring);
Value stringLength(LionVm* vm, ObjString* string);
Value stringSub(LionVm* vm, ObjString* string, size_t start, size_t end);

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

Value newModule(LionVm* vm);

static inline Value objectToValueOfTag(void* objptr, uint64_t tag){
    ValueBit vb;
    vb.asUint64 = (intptr_t) objptr;
    vb.asUint64 &= OBJPOINTER_MASK;
    vb.asUint64 |= (tag | QNAN | SIGN_BIT);
    return vb.value;
}
static inline Value numToValue(double num){
    ValueBit vb;
    vb.asDouble = num;
    return vb.value;
}

static inline double valueToNum(Value value){
    ValueBit vb;
    vb.value = value;
    return vb.asDouble;
}

#define objStringToValue(obj)       objectToValueOfTag(obj, TAG_STRING)
#define objClassToValue(obj)        objectToValueOfTag(obj, TAG_CLASS)
#define objClosureToValue(obj)      objectToValueOfTag(obj, TAG_CLOSURE)
#define objFiberToValue(obj)        objectToValueOfTag(obj, TAG_FIBER)
#define objFnToValue(obj)           objectToValueOfTag(obj, TAG_FN)
#define objForeignToValue(obj)      objectToValueOfTag(obj, TAG_FOREIGH)
#define objInstanceToValue(obj)     objectToValueOfTag(obj, TAG_INSTANCE)
#define objListToValue(obj)         objectToValueOfTag(obj, TAG_LIST)
#define objMapToValue(obj)          objectToValueOfTag(obj, TAG_MAP)

#define valueToObj(value) (intptr_t) (value & OBJPOINTER_MASK)

#define valueToString(value)    cast(ObjString, valueToObj(value))
#define valueToClass(value)     cast(ObjClass,  valueToObj(value))
#define valueToClosure(value)   cast(ObjClosure,  valueToObj(value))
#define valueToFiber(value)     cast(ObjFiber,  valueToObj(value))
#define valueToForeign(value)   cast(ObjForeign,  valueToObj(value))
#define valueToInstance(value)  cast(ObjInstance,  valueToObj(value))
#define valueToList(value)      cast(ObjList,  valueToObj(value))
#define valueToMap(value)       cast(ObjMap,  valueToObj(value))
#endif

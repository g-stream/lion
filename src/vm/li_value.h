#ifndef li_value_h
#define li_value_h
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "li_util.h"
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

typedef uint32_t instruction_t;
typedef uint64_t Value;
typedef Value* stkid_t;
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


/*  [x]   [11111111111][xxxx][11111111 11111111 11111111 11111111 11111111 11111111]
 *  [sign][nan bits   ][tag ][48bits for object pointer                            ]
 *  sign bit to indecate if the value is a collectable object
 */
/* singleton value*/
#define TAG_NAN         ((uint64_t)0 << 48)
#define TAG_NIL         ((uint64_t)1 << 48)
#define TAG_FALSE       ((uint64_t)2 << 48)
#define TAG_TRUE        ((uint64_t)3 << 48)
#define TAG_UNDEF       ((uint64_t)4 << 48)

/*object value which can be collected*/
#define TAG_STRING      (((uint64_t)1 << 48)|SIGN_BIT)
#define TAG_CLASS       (((uint64_t)2 << 48)|SIGN_BIT)
#define TAG_CLOSURE     (((uint64_t)3 << 48)|SIGN_BIT)
#define TAG_UPVALUE     (((uint64_t)4 << 48)|SIGN_BIT)
#define TAG_FN          (((uint64_t)5 << 48)|SIGN_BIT)
#define TAG_FOREIGN     (((uint64_t)6 << 48)|SIGN_BIT)
#define TAG_INSTANCE    (((uint64_t)7 << 48)|SIGN_BIT)
#define TAG_LIST        (((uint64_t)8 << 48)|SIGN_BIT)
#define TAG_MAP         (((uint64_t)9 << 48)|SIGN_BIT)

// Masks out the tag bits used to identify the  tag.
#define MASK_TAG      (((uint64_t)15 << 48)|SIGN_BIT)

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
#define IS_NIL(value)       ((value) == NIL_VAL)
#define IS_UNDEF(value)     ((value) == UNDEF_VAL)

#define IS_STRING(value)    (IS_OBJ(value)&&(GET_TAG(value) == TAG_STRING))
#define IS_CLASS(value)     (IS_OBJ(value)&&(GET_TAG(value) == TAG_CLASS))
#define IS_CLOSURE(value)   (IS_OBJ(value)&&(GET_TAG(value) == TAG_CLOSURE))
#define IS_UPVALUE(value)   (IS_OBJ(value)&&(GET_TAG(value) == TAG_UPVALUE))
#define IS_FN(value)        (IS_OBJ(value)&&(GET_TAG(value) == TAG_FN))
#define IS_FOREIGN(value)   (IS_OBJ(value)&&(GET_TAG(value) == TAG_FOREIGN))
#define IS_INSTANCE(value)  (IS_OBJ(value)&&(GET_TAG(value) == TAG_INSTANCE))
#define IS_LIST(value)      (IS_OBJ(value)&&(GET_TAG(value) == TAG_LIST))
#define IS_MAP(value)       (IS_OBJ(value)&&(GET_TAG(value) == TAG_MAP))

#define GET_TAG(value) ((uint64_t)((value)&(MASK_TAG)))





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

void printValue(Value v);

bool liEqual(Value l, Value r);
bool liNEqual(Value l, Value r);

Value liNumNeg(Value v);
Value liNumAdd(Value l, Value r);
Value liNumMinus(Value l, Value r);
Value liNumMulply(Value l, Value r);
Value liNumDiv(Value l, Value r);
Value liNumPow(Value l, Value r);
Value liNumIDiv(Value l, Value r);
Value liNumMod(Value l, Value r);

Value liBitNeg(Value v);
Value liBitAnd(Value l, Value r);
Value liBitOr(Value l, Value r);
Value liBitXor(Value l, Value r);
Value liBitShiftL(Value l, Value r);
Value liBitShiftR(Value l, Value r);

Value liBoolAnd(Value l, Value r);
Value liBoolOr(Value l, Value r);
Value liBoolXor(Value l, Value r);





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
    int num_args;
    instruction_t* code;
    size_t num_code;
}ObjFn;

typedef struct sObjClass
{
    struct sObjClass* superclass;
    size_t numFields;
    
    
    ObjString* name;
} ObjClass;

typedef struct sObjClosure
{
    ObjFn* fn;
    ObjUpvalue* upvalues;
    instruction_t* code;
    int code_size;
} ObjClosure;

typedef struct sCallFrame {
    stkid_t stack,stack_top;
    struct sCallFrame *pre, *next;
    ObjClosure* closure;
    Value* stack_start;
} CallFrame;

typedef struct sObjFiber{
    Value* stack;
    Value* stack_top;
    size_t stack_capacity;
    CallFrame* frame;
    CallFrame* base_frame;
    // The number of frames currently in use in [frames].
    size_t num_frames;
    size_t frame_capacity;
    ObjUpvalue* open_upvalues;
    struct sObjFiber* caller;
    Value error;
}ObjFiber;

typedef struct sObjForeign{
    
}ObjForeign;

typedef struct sObjInstance{
    
}ObjInstance;

DECLARE_BUFFER(Value, Value);

typedef struct sMapItem{
    Value key;
    Value value;
    uint32_t hash;
} MapItem;


typedef struct sOjbList{
    ValueBuffer elements;
}ObjList;

typedef struct sObjMap{
    size_t count;
    size_t capacity;
    MapItem* items;
}ObjMap;



typedef struct sObjModule{
    
}ObjModule;




typedef struct sLionVm {
    
    ObjClass* bool_class;
    ObjClass* class_class;
    ObjClass* fiber_class;
    ObjClass* fn_class;
    ObjClass* list_class;
    ObjClass* map_class;
    ObjClass* null_class;
    ObjClass* num_class;
    ObjClass* object_class;
    ObjClass* range_class;
    ObjClass* string_class;

    ObjFiber* fiber;


    ObjMap* modules;
    
    size_t bytes_allocated;

    SymbolTable methodNames;
} LionVm;

LionVm* liNewVm();
ObjFiber* liNewFiber();

Value liNewString(LionVm* vm, const char* cstring);
Value liStringLength(LionVm* vm, ObjString* string);
Value liStringSub(LionVm* vm, ObjString* string, size_t start, size_t end);
void  printString(Value v);

Value liNewUpvalue(LionVm* vm);



Value liNewClass(LionVm* vm);
Value liClassBindSuper(LionVm* vm, ObjClass* class, ObjClass* super_class);

Value liNewList(LionVm* vm, uint32_t size);
void  liListInsert(LionVm* vm, ObjList* list, Value value, uint32_t index);
void  liListRemove(LionVm* vm, ObjList* list, Value index);
Value liListSize(LionVm* vm, ObjList* list);

Value liNewMap(LionVm* vm, uint32_t num_items);
bool  liExistKey(LionVm* vm, ObjMap* map, Value key);
void  liMapUpdate(LionVm* vm, ObjMap* map, Value key, Value value);
void  liMapRemove(LionVm* vm, ObjMap* map, Value key);
int   liMapSize(LionVm* vm, ObjMap* map);

Value liNewModule(LionVm* vm);

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

#define valueToString(value)    cast(ObjString*, valueToObj(value))
#define valueToClass(value)     cast(ObjClass*,  valueToObj(value))
#define valueToClosure(value)   cast(ObjClosure*,  valueToObj(value))
#define valueToFiber(value)     cast(ObjFiber*,  valueToObj(value))
#define valueToForeign(value)   cast(ObjForeign*,  valueToObj(value))
#define valueToInstance(value)  cast(ObjInstance*,  valueToObj(value))
#define valueToList(value)      cast(ObjList*,  valueToObj(value))
#define valueToMap(value)       cast(ObjMap*,  valueToObj(value))


#endif

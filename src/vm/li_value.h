#ifndef li_value_h
#define li_value_h
#include <stdio.h>
#include <stdint.h>
#include <string.h>
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

// Tag values for the different singleton values.
/*
 * for double type, if the 52nd to 63rd bits is setted, it is a nan value. So we use 
 * other bits of a nan value to reperesent our different type.
 * Use the three bits before the bits setted for nan to reperesent singleton value types below
 * If the top most bit is setted, the value is a class, the lowest 32 bit is used to reperesent the pointer to the value
 */
#define TAG_NAN       ((uint64_t)0 << 49)
#define TAG_NULL      ((uint64_t)1 << 49)
#define TAG_FALSE     ((uint64_t)2 << 49)
#define TAG_TRUE      ((uint64_t)3 << 49)
#define TAG_UNDEF     ((uint64_t)4 << 49)
#define TAG_STR       ((uint64_t)5 << 49)
#define TAG_UNUSED3   ((uint64_t)6 << 49)
#define TAG_UNUSED4   ((uint64_t)7 << 49)

// Masks out the tag bits used to identify the singleton value.
#define MASK_TAG      ((uint64_t)7 << 49)

// Singleton values.
#define NULL_VAL      ((Value)(uint64_t)(QNAN | TAG_NULL))
#define FALSE_VAL     ((Value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL      ((Value)(uint64_t)(QNAN | TAG_TRUE))
#define UNDEF_VAL     ((Value)(uint64_t)(QNAN | TAG_UNDEF))

#define IS_NUM(value) (((value) & QNAN) != QNAN)
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_FALSE(value)     ((value) == FALSE_VAL)
#define IS_TRUE(value)      ((value) == TRUE_VAL)
#define IS_NULL(value)      ((value) == NULL_VAL)
#define IS_UNDEF(value)     ((value) == UNDEFINED_VAL)
#define IS_STR(value)       (((value)&(QNAN | MASK_TAG | SIGN_BIT)) == (QNAN | TAG_STR))




// Value -> 0 or 1.
#define AS_BOOL(value) ((value) == TRUE_VAL)

// Value -> Obj*.
#define AS_OBJ(value) ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))
#define AS_STR_INDEX(value) ((uint32_t)((value) & ~(SIGN_BIT|QNAN|MASK_TAG)))


typedef enum{
    VAL_NULL,
    VAL_FALSE,
    VAL_TRUE,
    VAL_NUM,
    VAL_STR,
    VAL_UNDEF,
    VAL_OBJ
} ValueType;


#define CommonHeader ObjClass *next; bool isDark; ObjType type

typedef struct sObjString{
    uint32_t length;
    uint32_t hash;
    char* content;
} ObjString;

typedef struct sObjClass{
}ObjClass;
typedef struct sObjClosure{
}ObjClosure;
typedef struct sObjFiber{
}ObjFiber;
typedef struct sObjFn{
}ObjFn;
typedef struct sObjForeign{
}ObjForeign;
typedef struct sObjInstance{
}ObjInstance;
typedef struct sOjbList{
}ObjList;
typedef struct sObjMap{
}ObjMap;
typedef struct sObjModule{
}ObjModule;
typedef struct sObjUpvalue{
}ObjUpvalue;



#endif

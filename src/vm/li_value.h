#ifndef li_value_h
#define li_value_h

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef uint32_t instruction_t
#define GET_OPCODE(code) ((code)>>26)
#define GET_REGA(code) (((code)&0x03fc0000)>>18)
#define GET_REGB(code) (((code)&0x0003fe00)>>9)
#define GET_REGC(code) ((code)& 0x000001ff)
#define GET_REGAx(code) ((code)&0x03ffffff)
#define GET_REGBx(code) ((code)&0x0003ffff)
#define GEN_CODE_ABC(op, A, B, C) ((instruction_t)((((op)&0x3f)<<26)|(((A)&0xff)<<18)|(((B)&0x1ff)<<9)|((C)&0x1ff)))
#define GEN_CODE_Ax(op, Ax) ((instruction_t)((((op)&0x3f)<<26)|(((Ax)&0x03ffffff))))
#define GEN_CODE_ABx(op, A, Bx) ((instruction_t)((((op)&0x3f)<<26)|(((A)&0xff)<<18)|((Bx)&0x0003ffff)))


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

typedef struct ObjClass ObjClass;


#define CommonHeader ObjClass *next; bool isDark; ObjType type


#define uint64_t Value;
#define SIGN_BIT ((uint64_t)1 << 63)
#define QNAN ((uint64_t)0X7ffc000000000000)

#define IS_NUM(value) (((value) & QNAN) != QNAN)

#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_FALSE(value)     ((value) == FALSE_VAL)
#define IS_NULL(value)      ((value) == NULL_VAL)
#define IS_UNDEF(value)     ((value) == UNDEFINED_VAL)
#define IS_STR(value)       (((value)&(QNAN | MASK_TAG | SIGN_BIT)) == (QNAN | TAG_STR))
// Masks out the tag bits used to identify the singleton value.
#define MASK_TAG ((uint64_t)7 << 49)

// Tag values for the different singleton values.
#define TAG_NAN       ((uint64_t)0 << 49)
#define TAG_NIL       ((uint64_t)1 << 49)
#define TAG_FALSE     ((uint64_t)2 << 49)
#define TAG_TRUE      ((uint64_t)3 << 49)
#define TAG_UNDEF     ((uint64_t)4 << 49)
#define TAG_STR       ((uint64_t)5 << 49)
#define TAG_UNUSED3   ((uint64_t)6 << 49)
#define TAG_UNUSED4   ((uint64_t)7 << 49)

// Value -> 0 or 1.
#define AS_BOOL(value) ((value) == TRUE_VAL)

// Value -> Obj*.
#define AS_OBJ(value) ((Obj*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))
#define AS_STR_INDEX(value) ((uint32_t)((value) & ~(SIGN_BIT|QNAN|MASK_TAG)))

// Singleton values.
#define NIL_VAL      ((Value)(uint64_t)(QNAN | TAG_NULL))
#define FALSE_VAL     ((Value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL      ((Value)(uint64_t)(QNAN | TAG_TRUE))
#define UNDEFINED_VAL ((Value)(uint64_t)(QNAN | TAG_UNDEFINED))



typedef enum{
    VAL_NIL,
    VAL_FALSE,
    VAL_TRUE,
    VAL_NUM,
    VAL_STR,
    VAL_UNDEF,
    VAL_OBJ
} ValueType;




#endif

#include "li_compiler.h"
#include "li_mem.h"
#include "li_config.h"
#include "li_vm.h"

#define MAX_LOCALS 256

#define MAX_UPVALUES 256

#define MAX_CONSTANTS (1 << 16)

#define MAX_JUMP (1 << 16)

#define MAX_INTERPOLATION_NESTING 8




/*
 * TOKEN lexer
 */

#define li_tokens TOKEN(LEFT_PAREN,       '(',       1)\
                  TOKEN(RIGHT_PAREN,      ')',       1)\
                  TOKEN(LEFT_BRACKET,     '[',       1)\
                  TOKEN(RIGHT_BRACKET,    ']',       1)\
                  TOKEN(LEFT_BRACE,       '{',       1)\
                  TOKEN(RIGHT_BRACE,      '}',       1)\
                  TOKEN(COLON,            ':',       1)\
                  TOKEN(DOT,              '.',       1)\
                  TOKEN(DOTDOT,           "..",      2)\
                  TOKEN(DOTDOTDOT,        "...",     3)\
                  TOKEN(COMMA,            ',',       1)\
                  TOKEN(STAR,             '*',       1)\
                  TOKEN(SLASH,            '/',       1)\
                  TOKEN(PERCETN,          '%',       1)\
                  TOKEN(BACKSLASH,        '\\',      1)\
                  TOKEN(PLUS,             '+',       1)\
                  TOKEN(MINUS,            '-',       1)\
                  TOKEN(LTLT,             "<<",      2)\
                  TOKEN(GTGT,             ">>",      2)\
                  TOKEN(LT,               '<',       1)\
                  TOKEN(GT,               '>',       1)\
                  TOKEN(LTEQ,             "<=",      2)\
                  TOKEN(GTEQ,             ">=",      2)\
                  TOKEN(EQ,               '=',       1)\
                  TOKEN(EQEQ,             "==",      2)\
                  TOKEN(BANGEQ,           "!=",      2)\
                  TOKEN(PIPE,             '|',       1)\
                  TOKEN(PIPEPIPE,         "||",      2)\
                  TOKEN(AMP,              '&',       1)\
                  TOKEN(AMPAMP,           "&&",      2)\
                  TOKEN(BANG,             '!',       1)\
                  TOKEN(TILDE,            '~',       1)\
                  TOKEN(QUESTION,         '?',       1)\
                  TOKEN(BREAK,            "break",   5)\
                  TOKEN(CONTINUE,         "continue",8)\
                  TOKEN(CLASS,            "",        0)\
                  TOKEN(IF,               "if",      2)\
                  TOKEN(ELSE,             "else",    4)\
                  TOKEN(FALSE,            "false",   5)\
                  TOKEN(TRUE,             "true",    4)\
                  TOKEN(IMPORT,           "import"  ,6)\
                  TOKEN(IS,               "is",      2)\
                  TOKEN(NIL,              "nil",    3)\
                  TOKEN(RETURN,           "return",  6)\
                  TOKEN(VAR,              "var",     3)\
                  TOKEN(WHILE,            "while",   5)\
                  TOKEN(FOR,              "for",     3)\
                  TOKEN(ARROR,            "->",      2)\
                  TOKEN(NAME,             "",        0)\
                  TOKEN(NUMBER,           "",        0)\
                  TOKEN(FIELD,            "",        0)\
                  TOKEN(STRING,           "",        0)\
                  TOKEN(LINE,             "",        0)\
                  TOKEN(ERROR,            "",        0)\
                  TOKEN(EOF,              "",        0)
        
/*
 *  get a series names -----  TOKEN_XXXX_CONTENT
 *  for one char token, use char to store its value
 *  for two or more char token, use const char* to store its value
*/

#define DEFINE_TOKEN_CONTENT0(token, c) 
#define DEFINE_TOKEN_CONTENT1(token, c) char TOKEN_##token##_CONTENT = c;
#define DEFINE_TOKEN_CONTENT_OTHER(token, str) const char* TOKEN_##token##_CONTENT = str;
#define DEFINE_TOKEN_CONTENT2(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT3(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT4(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT5(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT6(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT7(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
#define DEFINE_TOKEN_CONTENT8(token, str) DEFINE_TOKEN_CONTENT_OTHER(token, str)
//get TOKEN_XXX_CONTENT as tokens' names
#define TOKEN(token, token_content, num) DEFINE_TOKEN_CONTENT##num(token, token_content)
li_tokens
#undef TOKEN

//get li_tokens_type
typedef enum {
#define TOKEN(token, token_content, num) TOKEN_##token,
li_tokens
#undef TOKEN
} TokenType;

#define DEFINE_COMPARE_WITH_TOKEN_XXX0(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX1(token, num) bool COMPARE_WITH_TOKEN_##token(const char* str) { return *str == TOKEN_##token##_CONTENT;}
#define DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num) bool COMPARE_WITH_TOKEN_##token(const char* str){\
                                                  for(int i = 0; i < num; i++){\
                                                      if(str[i] != TOKEN_##token##_CONTENT[i])\
                                                          return false;\
                                                  }\
                                                  return true;}
                                                  
#define DEFINE_COMPARE_WITH_TOKEN_XXX2(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX3(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX4(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX5(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX6(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX7(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)
#define DEFINE_COMPARE_WITH_TOKEN_XXX8(token, num) DEFINE_COMPARE_WITH_TOKEN_XXX_OTHER(token, num)

//define token compare functions, remember these functions won't check the string's length is less than num
#define TOKEN(token, token_content, num) DEFINE_COMPARE_WITH_TOKEN_XXX##num(token, num)
li_tokens
#undef TOKEN


const char* token_name[] = {
#define TOKEN(token, token_content, num) CVT_TOKEN_NAME(TOKEN_##token)
#define CVT_TOKEN_NAME(token_name) #token_name ,
li_tokens
#undef TOKEN
#undef CVT_TOKEN_NAME
};


typedef struct{
    size_t start;
    size_t end;
} StringSlice;

typedef struct sLexerState{
    const char*  src;
    const char*  cur;
    size_t       numRow;
    size_t       numCol;
    TokenType    curToken;
    StringSlice  stringSlice;
    TokenType    nextToken;
}LexerState;

void initLexerState(LexerState* lexer_state){
    lexer_state->src = "for i in a print a";
}

void printStringSlice(LexerState* lexer_state, StringSlice slice){
    const char* p = lexer_state->src;
    p += slice.start;
    int n = slice.end-slice.start;
    while(n--){
        putchar(*(p++));
    }
}

bool isAlphbeta(const char* c){
    if('a' <= *c && *c <= 'z' || 'A' <= *c && *c <= 'Z')
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
bool isCharOfVarStart(const char* c){
    if(*c == '_' || isAlphbeta(c) || isNum(c))
        return true;
    return false;
}




void test_li_ocmpiler(){
    LexerState lexer_state;
    initLexerState(&lexer_state);
    StringSlice slice = {3,5};
    printStringSlice(&lexer_state, slice);
}



/*
 * OPCODE operator
 */

typedef uint32_t instruction_t;
#define cast(type, v) ((type) (v))
/*
** size and position of opcode arguments.
*/
#define SIZE_C		9
#define SIZE_B		9
#define SIZE_Bx		(SIZE_C + SIZE_B)
#define SIZE_A		8
#define SIZE_Ax		(SIZE_C + SIZE_B + SIZE_A)

#define SIZE_OP		6

#define POS_OP		0
#define POS_A		(POS_OP + SIZE_OP)
#define POS_C		(POS_A + SIZE_A)
#define POS_B		(POS_C + SIZE_C)
#define POS_Bx		POS_C
#define POS_Ax		POS_A
/*
** limits for opcode arguments.
** we use (signed) int to manipulate most arguments,
** so they must fit in LUAI_BITSINT-1 bits (-1 for sign)
*/
#define MAXARG_Bx        ((1<<SIZE_Bx)-1)
#define MAXARG_sBx        (MAXARG_Bx>>1)         /* 'sBx' is signed */

#define MAXARG_Ax	((1<<SIZE_Ax)-1)


#define MAXARG_A        ((1<<SIZE_A)-1)
#define MAXARG_B        ((1<<SIZE_B)-1)
#define MAXARG_C        ((1<<SIZE_C)-1)


/* creates a mask with 'n' 1 bits at position 'p' */
#define MASK1(n,p)	((~((~(instruction_t)0)<<(n)))<<(p))

/* creates a mask with 'n' 0 bits at position 'p' */
#define MASK0(n,p)	(~MASK1(n,p))

/*
** the following macros help to manipulate instruction_t
*/

#define GET_OPCODE(i)	(cast(OpCode, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))
#define SET_OPCODE(i,o)	((i) = (((i)&MASK0(SIZE_OP,POS_OP)) | \
		((cast(instruction_t, o)<<POS_OP)&MASK1(SIZE_OP,POS_OP))))

#define getarg(i,pos,size)	(cast(int, ((i)>>pos) & MASK1(size,0)))
#define setarg(i,v,pos,size)	((i) = (((i)&MASK0(size,pos)) | \
                ((cast(instruction_t, v)<<pos)&MASK1(size,pos))))

#define GETARG_A(i)	getarg(i, POS_A, SIZE_A)
#define SETARG_A(i,v)	setarg(i, v, POS_A, SIZE_A)

#define GETARG_B(i)	getarg(i, POS_B, SIZE_B)
#define SETARG_B(i,v)	setarg(i, v, POS_B, SIZE_B)

#define GETARG_C(i)	getarg(i, POS_C, SIZE_C)
#define SETARG_C(i,v)	setarg(i, v, POS_C, SIZE_C)

#define GETARG_Bx(i)	getarg(i, POS_Bx, SIZE_Bx)
#define SETARG_Bx(i,v)	setarg(i, v, POS_Bx, SIZE_Bx)

#define GETARG_Ax(i)	getarg(i, POS_Ax, SIZE_Ax)
#define SETARG_Ax(i,v)	setarg(i, v, POS_Ax, SIZE_Ax)

#define GETARG_sBx(i)	(GETARG_Bx(i)-MAXARG_sBx)
#define SETARG_sBx(i,b)	SETARG_Bx((i),cast(unsigned int, (b)+MAXARG_sBx))


#define GEN_ABC(o,a,b,c)	((cast(instruction_t, o)<<POS_OP) \
			| (cast(instruction_t, a)<<POS_A) \
			| (cast(instruction_t, b)<<POS_B) \
			| (cast(instruction_t, c)<<POS_C))

#define GEN_ABx(o,a,bc)	((cast(instruction_t, o)<<POS_OP) \
			| (cast(instruction_t, a)<<POS_A) \
			| (cast(instruction_t, bc)<<POS_Bx))

#define GEN_Ax(o,a)		((cast(instruction_t, o)<<POS_OP) \
			| (cast(instruction_t, a)<<POS_Ax))

/*
** Macros to operate RK indices
*/

/* this bit 1 means constant (0 means register) */
#define BITRK		(1 << (SIZE_B - 1))

/* test whether value is a constant */
#define ISK(x)		((x) & BITRK)

/* gets the index of the constant */
#define INDEXK(r)	((int)(r) & ~BITRK)

#if !defined(MAXINDEXRK)  /* (for debugging only) */
#define MAXINDEXRK	(BITRK - 1)

/* code a constant index as a RK value */
#define RKASK(x)	((x) | BITRK)


/*
** invalid register that fits in 8 bits
*/
#define NO_REG		MAXARG_A

#define OPCODE_ABC 0
#define OPCODE_ABx 1
#define OPCODE_AsBx 1
#define OPCODE_Ax  2


#define li_opcodes OPCODE(NULL,         2)\
                   OPCODE(ADD,          0) /*	A B C	R(A) := RK(B) + RK(C)				*/\
                   OPCODE(SUB,          0) /*	A B C	R(A) := RK(B) - RK(C)				*/\
                   OPCODE(MUL,          0) /*	A B C	R(A) := RK(B) * RK(C)				*/\
                   OPCODE(MOD,          0) /*	A B C	R(A) := RK(B) % RK(C)				*/\
                   OPCODE(POW,          0) /*	A B C	R(A) := RK(B) ^ RK(C)				*/\
                   OPCODE(DIV,          0) /*	A B C	R(A) := RK(B) / RK(C)				*/\
                   OPCODE(IDIV,         0) /*	A B C	R(A) := RK(B) // RK(C)				*/\
                   OPCODE(BAND,         0) /*	A B C	R(A) := RK(B) & RK(C)				*/\
                   OPCODE(BOR,          0) /*	A B C	R(A) := RK(B) | RK(C)				*/\
                   OPCODE(BXOR,         0) /*	A B C	R(A) := RK(B) ~ RK(C)				*/\
                   OPCODE(SHL,          0) /*	A B C	R(A) := RK(B) << RK(C)				*/\
                   OPCODE(SHR,          0) /*	A B C	R(A) := RK(B) >> RK(C)				*/\
                   OPCODE(UNM,          1) /*	A B	R(A) := -R(B)					*/\
                   OPCODE(BNOT,         1) /*	A B	R(A) := ~R(B)					*/\
                   OPCODE(NOT,          1) /*	A B	R(A) := not R(B)				*/
                   



                   
#endif

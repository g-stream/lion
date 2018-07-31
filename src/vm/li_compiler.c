#include "li_compiler.h"
#include "li_mem.h"
#include "li_config.h"
#include "li_vm.h"
#include "li_value.h"
#include "li_compiler.h"
#include "stdlib.h"
#include "li_debug.h"
#include "li_util.h"
#include "stdlib.h"
#include "stdio.h"
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

#define TOKEN(token, token_content, num) const int TOKEN_##token##_LENGTH = num ;
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

typedef struct sToken{
    TokenType type;
    Value     value;
} Token;
typedef struct sLexerState{
    const char*  src;
    const char*  cur;
    const char*  curTokenStart;
    const char*  curRow;
    size_t       numRow;
    size_t       numCol;
    Token        curToken;
    Token        nextToken;
}LexerState;


#define cur_pt (lexer_state->cur)
#define currow_pt (lexer_state->curRow)
#define cur_char (*cur_pt)
#define next_char (*(cur_pt + 1))
#define start_pt  (lexer_state->curTokenStart)
void initLexerState(LexerState* lexer_state){
    lexer_state->src = "1234e-1";
    lexer_state->curRow = lexer_state->src;
    lexer_state->cur = lexer_state->src;
}

void lexerErrorWithHightlight(LexerState* lexer_state, ErrorType err, const char* info){
    printError(err, info);
    putchar('\n');
    size_t pos = cur_pt - currow_pt;
    const int size = 1000;
    char row[size];
    const char* lp = currow_pt;
    int i = 0;
    while(*lp != '\0' && *lp != '\n' && *lp != '\r'){
        row[i++] = *(lp++);
    }
    row[i++] = '\0';
    assert(i < size);
    lexer_state->curToken.type = TOKEN_ERROR;
    lexer_state->curToken.value = UNDEF_VAL;
    printWithHighlight(row, pos);
}

void skip(LexerState* lexer_state){
    if(isSkip(cur_pt)){
        if(cur_char == '\n'){
            if(next_char == '\r')
                cur_pt++;
            lexer_state->curRow = cur_pt;
            lexer_state->numRow++;
            lexer_state->curToken.type = TOKEN_LINE;
            lexer_state->curToken.value = UNDEF_VAL;
            return;
        }
        if(cur_char == '\r'){
            if(next_char == '\n')
                cur_pt++;
            lexer_state->curToken.type = TOKEN_LINE;
            lexer_state->curToken.value = UNDEF_VAL;
            return;
        }
        cur_pt++;
    }
}

void skipComment(LexerState* lexer_state){
    if(cur_char == '/' && next_char == '/'){
        while(cur_char != '\0' || cur_char != '\n')
            cur_pt++;
        skip(lexer_state);
    }
}

#define SET_LEXER_STATE_TOKEN(token) lexer_state->curToken.type = token
#define SET_LEXER_STATE_VALUE(v)     lexer_state->curToken.value = v
#define INCREASE_LEXER_CUR_POINTET(token) lexer_state->cur += token##_LENGTH
#define MAKE_REGULAR_TOKEN(token) SET_LEXER_STATE_TOKEN(token); SET_LEXER_STATE_VALUE(UNDEF_VAL); INCREASE_LEXER_CUR_POINTET(token)
#define MAKE_TOKEN(token, v) SET_LEXER_STATE_TOKEN(token); SET_LEXER_STATE_VALUE(v); INCREASE_LEXER_CUR_POINTET(token)

void readVar(LexerState* lexer_state){
    if(!isCharOfVar(cur_pt)){
        lexerErrorWithHightlight(lexer_state, SyntaxError, "invalid var name");
    }
    
}

void readVarOrkeyword(LexerState* lexer_state){
    switch(cur_char){
        case '{':
            MAKE_REGULAR_TOKEN(TOKEN_LEFT_BRACE);return;
        case '}':
            MAKE_REGULAR_TOKEN(TOKEN_RIGHT_BRACE);return;
        case '[':
            MAKE_REGULAR_TOKEN(TOKEN_LEFT_BRACKET);return;
        case ']':
            MAKE_REGULAR_TOKEN(TOKEN_RIGHT_BRACKET);return;
        case '(':
            MAKE_REGULAR_TOKEN(TOKEN_LEFT_PAREN);return;
        case ')':
            MAKE_REGULAR_TOKEN(TOKEN_RIGHT_PAREN);return;
        case ':':
            MAKE_REGULAR_TOKEN(TOKEN_COMMA);return;
        case '.':
            if(COMPARE_WITH_TOKEN_DOTDOTDOT(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_DOTDOTDOT);return;
            }else if(COMPARE_WITH_TOKEN_DOTDOT(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_DOTDOT);return;
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_DOT);return;
            }
            return;
        case ',':
            MAKE_REGULAR_TOKEN(TOKEN_COMMA);return;
        case '*':
            MAKE_REGULAR_TOKEN(TOKEN_STAR);return;
        case '/':
            MAKE_REGULAR_TOKEN(TOKEN_SLASH);return;
        case '%':
            MAKE_REGULAR_TOKEN(TOKEN_PERCETN);return;
        case '\\':
            MAKE_REGULAR_TOKEN(TOKEN_BACKSLASH);return;
        case '+':
            MAKE_REGULAR_TOKEN(TOKEN_PLUS);return;
        case '-':
            if(COMPARE_WITH_TOKEN_ARROR(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_ARROR);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_MINUS);
            }
            return;
        case '>':
            if(COMPARE_WITH_TOKEN_GTGT(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_GTGT);
            }else if(COMPARE_WITH_TOKEN_GTEQ(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_GTEQ);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_GT);
            }
            return;
        case '<':
            if(COMPARE_WITH_TOKEN_LTLT(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_LTLT);
            }else if(COMPARE_WITH_TOKEN_LTEQ(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_LTEQ);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_LT);
            }
            return;
        case '!':
            if(COMPARE_WITH_TOKEN_BANGEQ(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_BANGEQ);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_BANG);
            }
            return;
        case '&':
            if(COMPARE_WITH_TOKEN_AMPAMP(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_AMPAMP);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_AMP);
            }
            return;
        case '|':
            if(COMPARE_WITH_TOKEN_PIPEPIPE(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_PIPEPIPE);
            }else{
                MAKE_REGULAR_TOKEN(TOKEN_PIPE);
            }
            return;
        case '~':
            MAKE_REGULAR_TOKEN(TOKEN_TILDE);return;
        case '?':
            MAKE_REGULAR_TOKEN(TOKEN_QUESTION);return;
        case 'b':
            if(COMPARE_WITH_TOKEN_BREAK(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_BREAK);
                return;
            }
        case 'c':
            if(COMPARE_WITH_TOKEN_CONTINUE(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_CONTINUE);
                return;
            }
        case 'i':
            if(COMPARE_WITH_TOKEN_IF(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_IF);
                return;
            } else if(COMPARE_WITH_TOKEN_IMPORT(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_IMPORT);
                return;
            }
        case 'e':
            if(COMPARE_WITH_TOKEN_ELSE(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_ELSE);
                return;
            }
        case 'f':
            if(COMPARE_WITH_TOKEN_FALSE(cur_pt)){
                MAKE_TOKEN(TOKEN_FALSE, FALSE_VAL);
                return;
            }else if(COMPARE_WITH_TOKEN_FOR(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_FOR);
                return;
            }
        case 't':
            if(COMPARE_WITH_TOKEN_TRUE(cur_pt)){
                MAKE_TOKEN(TOKEN_TRUE, TRUE_VAL);
                return;
            }
        case 'n':
            if(COMPARE_WITH_TOKEN_NIL(cur_pt)){
                MAKE_TOKEN(TOKEN_NIL, NIL_VAL);
                return;
            }
        case 'r':
            if(COMPARE_WITH_TOKEN_RETURN(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_RETURN);
                return;
            }
        case 'v':
            if(COMPARE_WITH_TOKEN_VAR(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_VAR);
                return;
            }
        case 'w':
            if(COMPARE_WITH_TOKEN_WHILE(cur_pt)){
                MAKE_REGULAR_TOKEN(TOKEN_WHILE);
                return;
            }
        default:
            readVar(lexer_state);
    }
            
}

void readNum(LexerState* lexer_state){
    double v = 0;
    start_pt = cur_pt;
    if(*cur_pt == '0' && isOneOfChars(cur_pt + 1, "xX")){
       cur_pt += 2;
       while(isHex(cur_pt)){
           v = v*16 + hexToNum(cur_pt++);
       }
       ValueBit value_bit;
       value_bit.asDouble = v;
       lexer_state->curToken.type  = TOKEN_NUMBER;
       lexer_state->curToken.value = value_bit.asUint64;
       if(isCharOfVar(cur_pt)){
          lexerErrorWithHightlight(lexer_state, SyntaxError, "must split a number");
          return;
       }
       return;
    }
    while(isNum(cur_pt)){
        cur_pt++;
    }
    if(cur_char == '.'){
        cur_pt++;
        while(isNum(cur_pt)){
            cur_pt++;
        }
    }
    if(isOneOfChars(cur_pt, "eE")){
        cur_pt++;
        if(isOneOfChars(cur_pt, "+-")){
            cur_pt++;
        }
        if(!isNum(cur_pt)){
            lexerErrorWithHightlight(lexer_state, SyntaxError, "lack exponent part");
            return;
        }
        while(isNum(cur_pt)) cur_pt++;
    }
    if(isCharOfVar(cur_pt)){
        lexerErrorWithHightlight(lexer_state, SyntaxError, "must split a number");
        return;
    }
    v = strtold(start_pt, NULL);
    ValueBit value_bit;
    value_bit.asDouble = v;
    lexer_state->curToken.type = TOKEN_NUMBER;
    lexer_state->curToken.value = value_bit.asUint64;
}

void readString(LexerState* lexer_state){
    
}
void readRawString(LexerState* lexer_state){
    
}

void test_li_ocmpiler(){
    LexerState lexer_state;
    initLexerState(&lexer_state);
    readNum(&lexer_state);
    ValueBit bit;
    bit.asUint64 = lexer_state.curToken.value;
    printf("%f", bit.asDouble);
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

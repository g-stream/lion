#include "li_compiler.h"
#include "li_mem.h"
#include "li_config.h"
#include "li_vm.h"
#include "li_value.h"
#include "stdlib.h"
#include "li_debug.h"
#include "li_util.h"
#include "stdlib.h"
#include "stdio.h"
#include "li_vm_macro.h"
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






#define OPCODE(name, effect) OPNAME(OP_##name)
#define OPNAME(name) #name,
const char* op_name[] = {
    li_opcodes
};
#undef OPNAME
#undef OPCODE


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

void test_opcode(int type, int op, int aa, int b, int c, int bx, int ax){
    instruction_t a;
    switch (type){
        case 1:
            a = GEN_ABC(op, aa, b, c);
            printf("op:%d, a:%d, b:%d, c:%d\n", GET_OPCODE(a), GETARG_A(a), GETARG_B(a), GETARG_C(a));
            break;
        case 2:
             a = GEN_ABx(op, aa, bx);
            printf("op:%d, a:%d, bx:%d\n", GET_OPCODE(a), GETARG_A(a), GETARG_Bx(a));
            break;
        case 3:
             a = GEN_Ax(op, ax);
            printf("op:%d, ax:%d\n", GET_OPCODE(a), GETARG_Ax(a));
            break;
    }
}

void test_li_compiler(){
    LionVm vm;
    const char* cstr = "asdf";
    Value str = liNewString(&vm, cstr);
    printString(str);
    printf("%s", op_name[OP_ADD]);
    
}


    

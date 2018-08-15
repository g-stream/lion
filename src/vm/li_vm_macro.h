#ifndef li_vm_macro_h
#define li_vm_macro_h


#define li_opcodes OPCODE(NULL,         0)\
                   OPCODE(ADD,          2) /*	A B C	R(A) := RK(B) + RK(C)				*/\
                   OPCODE(SUB,          2) /*	A B C	R(A) := RK(B) - RK(C)				*/\
                   OPCODE(MUL,          2) /*	A B C	R(A) := RK(B) * RK(C)				*/\
                   OPCODE(MOD,          2) /*	A B C	R(A) := RK(B) % RK(C)				*/\
                   OPCODE(POW,          2) /*	A B C	R(A) := RK(B) ^ RK(C)				*/\
                   OPCODE(DIV,          2) /*	A B C	R(A) := RK(B) / RK(C)				*/\
                   OPCODE(IDIV,         2) /*	A B C	R(A) := RK(B) // RK(C)				*/\
                   OPCODE(BAND,         2) /*	A B C	R(A) := RK(B) & RK(C)				*/\
                   OPCODE(BOR,          2) /*	A B C	R(A) := RK(B) | RK(C)				*/\
                   OPCODE(BXOR,         2) /*	A B C	R(A) := RK(B) ~ RK(C)				*/\
                   OPCODE(SHL,          2) /*	A B C	R(A) := RK(B) << RK(C)				*/\
                   OPCODE(SHR,          2) /*	A B C	R(A) := RK(B) >> RK(C)				*/\
                   OPCODE(UNM,          1) /*	A B	R(A) := -R(B)					*/\
                   OPCODE(BNOT,         1) /*	A B	R(A) := ~R(B)					*/\
                   OPCODE(NOT,          1) /*	A B	R(A) := not R(B)				*/\
                   OPCODE(HALT,         0) /*   Halt the virtual machine*/

#define OPCODE(name, effect) OP_##name ,
                   
typedef enum OP{
    li_opcodes
}OP_CODE;
#undef OPCODE

/*
 * OPCODE operator
 */


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

#define MASK_OP MASK1(SIZE_OP, POS_OP)
#define MASK_A  MASK1(SIZE_A, POS_A)
#define MASK_B  MASK1(SIZE_B, POS_B)
#define MASK_C  MASK1(SIZE_C, POS_C)
#define MASK_Bx MASK1(SIZE_Bx, POS_Bx)
#define MASK_Ax MASK1(SIZE_Ax, POS_Ax)

/* creates a mask with 'n' 0 bits at position 'p' */
#define MASK0(n,p)	(~MASK1(n,p))

/*
** the following macros help to manipulate instruction_t
*/

#define GET_OPCODE(i)	(cast(int, ((i)>>POS_OP) & MASK1(SIZE_OP,0)))
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



#define GEN_ABC(o,a,b,c)	(((cast(instruction_t, o)<<POS_OP)&MASK_OP) \
			| ((cast(instruction_t, a)<<POS_A) & MASK_A) \
			| ((cast(instruction_t, b)<<POS_B) & MASK_B)\
			| ((cast(instruction_t, c)<<POS_C) & MASK_C))

#define GEN_ABx(o,a,bc)	(((cast(instruction_t, o)<<POS_OP) & MASK_OP) \
			| ((cast(instruction_t, a)<<POS_A) & MASK_A)\
			| ((cast(instruction_t, bc)<<POS_Bx) & MASK_Bx))

#define GEN_Ax(o,a)		(((cast(instruction_t, o)<<POS_OP) & MASK_OP) \
			| ((cast(instruction_t, a)<<POS_Ax) & MASK_Ax))

/*
** Macros to operate RK indices
*/

/* this bit 1 means constant (0 means register) */
#define BITRK		(1 << (SIZE_B - 1))

/* test whether value is a constant */
#define ISK(x)		((x) & BITRK)

/* gets the index of the constant */
#define INDEXK(r)	((int)(r) & ~BITRK)



/*
** invalid register that fits in 8 bits
*/
#define NO_REG		MAXARG_A

#define OPCODE_ABC 0
#define OPCODE_ABx 1
#define OPCODE_AsBx 1
#define OPCODE_Ax  2

#endif

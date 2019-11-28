#ifndef _BASE_H_
#define _BASE_H_

#define ADDR_LEN        32      /* the length of pc address */
#define DATA_LEN        32      /* the length of data */
#define LINE_SIZE       64      /* the size of cacheline, which means \
                                   there are 64 bytes in a cacheline */

typedef unsigned long   Addr;
typedef unsigned char   Data;
typedef int             Cycle;

/* there are 10 insts, which need 4-bit to determine which inst is used;
 * thus, 6 insts are reserved */
enum class InstOp { ADD, SUB, MUL, DIV, LDR, STR, MOV, JMP, BEQ, NOP };
/* there are 16 ISA registers, which need 4-bit */
enum class ISAReg { R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, \
                    R10, R11, R12, R13, R14, R15 };
/* the  physical registers are not define here because i want to make \
 * them customizable */

/* cache line structure */
typedef struct _CacheLine {
    Addr tag;
    bool invalid;
    Data data[LINE_SIZE];
} CacheLine;

/* instruction infomation structure */
typedef struct _InstInfo {
    InstOp op;                              /* operation type */
    ISAReg reg_dst, reg_src1, reg_src2;     /* destination register & source registers */
    bool imdt_used;                         /* is ther inst uses immediate */
    unsigned short imdt;                    /* the value of immediate */
} InstInfo;

/* renamed instruction infomation structure */
typedef struct _RenamedInstInfo {
	InstOp op;                              /* operation type */
	int reg_dst, reg_src1, reg_src2;     /* destination register & source registers */
	bool imdt_used;                         /* is ther inst uses immediate */
	unsigned short imdt;                    /* the value of immediate */
} RenamedInstInfo;

/* some parameters of the instruction */
/* 31              16 15  12 11   8 7    4 3    0
 * [****************] [****] [****] [****] [****]
 *     immediate       src2   src1    dst    op
 * */
#define OPERATION_MASK          0xF
#define OPERATION_SHIFT         0
#define REG_DST_MASK            0xF0
#define REG_DST_SHIFT           4
#define REG_SRC1_MASK           0xF00
#define REG_SRC1_SHIFT          8
#define REG_SRC2_MASK           0xF000
#define REG_SRC2_SHIFT          12
#define IMDT_MASK               0xFFFF0000
#define IMDT_SHIFT              16

/* align the address (4-byte) */
Addr align(Addr addr);
/* align the address (one cacheline) */
Addr alignCacheLine(Addr addr);

/* calculate the bits needed */
int log2(int x);


/* range line for print */
extern const char* range_line;

#endif
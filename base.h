#ifndef _BASE_H_
#define _BASE_H_

#define ADDR_WIDTH  32
#define LINE_SIZE	64

typedef unsigned long Addr;
typedef unsigned char Data;
typedef int Cycle;

typedef struct CacheLine {
	unsigned long tag;
	unsigned char data[64];
}CacheLine;

#define LINE_ALIGNED(addr) (addr & (LINE_SIZE - 1))

enum class InstOp { ADD, SUB, MUL, DIV, LDR, STR, JMP, NOP };
enum class InstDataType { INT, FLOAT };
enum class InstReg { R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, RX };

typedef struct instInfo {
	InstOp op;								// opearation type
	InstDataType dt;						// data operated type
	InstReg reg_src1, reg_src2, reg_dst;	// used registers
	bool imdt;								// whether use immediate
}instInfo;


/*
* 31            18 17   13  12    8  7     3  2   0
* [xxxxxxxxxxxxxx] [xxxxx]  [xxxxx]  [xxxxx]  [xxx]
*    immediate     reg_dst  reg_src2 reg_src1 operation
*
*/
#define OPERATION_MASK			0x7
#define DATATYPE_MASK_SRC1		0x80
#define DATATYPE_MASK_SRC2		0x1000
#define DATATYPE_MASK_DST		0x20000
#define REGISTER_MASK_SRC1		0x78
#define REGISTER_SHIFT_SRC1		3
#define REGISTER_MASK_SRC2		0xF00
#define REGISTER_SHIFT_SRC2		8
#define REGISTER_MASK_DST		0x1E000
#define REGISTER_SHIFT_DST		13
#define IMMEDIATE_MASK			0xFFFC0000
#define IMMEDIATE_SHIFT			18

#endif
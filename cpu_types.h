/*
 * cpu_types.h
 *
 *  Created on: 31 Oct 2019
 *      Author: roget
 */

#include <iostream>
#include <vector>
#include <iterator>
#include <map>

#ifndef CPU_TYPES_H_
#define CPU_TYPES_H_

#define REG_SIZE 33

typedef enum {
	no_mode = -1,
	data,
	text,
	MAX_MODES
} cpu_loader_mode_t;

const char *mode_strings[] = {
		"data",
		"text",
};

// 16-bit cpu atm
typedef enum {
	r0 = 0, r1, r2, r3,
	r4, r5, r6, r7,
	r8, r9, r10, r11,
	r12, r13, r14, r15,
	cmp, halt,
	MAX_OPREG
} opreg_t;


typedef enum {
	NOP = 0,
	ADD, ADDI, SUB, SUBI,
	MULT, DIVD,
	LD, LDI, STO,
	CMP, BLT, BGT, BEQ,
	MOV, J,
	HALT,
	MAX_INSTRUCTIONS
}instructions_t;

std::map<int, int> instruction_args;

// TODO: mov instruction
const char *inst_string[MAX_INSTRUCTIONS] = {
	"nop",
	"add", "addi", "sub", "subi",
	"mult", "divd",
	"ld", "ldi", "sto",
	"cmp", "blt", "bgt", "beq",
	"mov", "j",
	"halt",
};

typedef struct {
	instructions_t opcode;
	int src0;
	int src1;
	int src2;
} decode_instruction_t;

typedef struct {
	opreg_t dest;
	uint32_t output;
	int write;

} wbreg_t;

typedef struct{
	int busy;
	opreg_t needs[2];
	decode_instruction_t rs;
}rs_t;

typedef struct {
	unsigned int pc;
	unsigned int clk;
	unsigned int instructions_executed;
	uint32_t reg_file[REG_SIZE];
	uint32_t reg_lock[REG_SIZE];
	char cir[4][40];
	uint8_t cir_ready;
	rs_t rs[8];
	decode_instruction_t decoded[8];
	unsigned int halt_reg;
	wbreg_t wbr[8];
}cpu_t;

typedef enum {
	EQ = 1 << 0,
	GT = 1 << 1,
	LT = 1 << 2
}comparison_t;



/*** Instantiation of CPU ***/
cpu_t cpu;
char *iram[128];
uint32_t dram[128];
std::map<std::string, int> label_map;
unsigned int iram_size = 0;

#endif /* CPU_TYPES_H_ */

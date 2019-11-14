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


typedef enum {
	r0 = 0, r1, r2, r3,
	r4, r5, r6, r7,
//	r8, r9, r10, r11,
	MAX_OPREG
} opreg_t;


typedef enum {
	NOP = 0,
	ADD, ADDI, SUB, SUBI,
	MULT, DIVD,
	LD, LDI, STO,
	CMP, BLT, BGT, BEQ,
	HALT,
	MAX_INSTRUCTIONS
}instructions_t;

std::map<int, int> instruction_args;


const char *inst_string[MAX_INSTRUCTIONS] = {
	"nop",
	"add", "addi", "sub", "subi",
	"mult", "divd",
	"ld", "ldi", "sto",
	"cmp", "blt", "bgt", "beq",
	"halt",
};

typedef struct {
	instructions_t opcode;
	opreg_t src0;
	int src0i;
	opreg_t src1;
	int src1i;
	opreg_t src2;
	int src2i;
} decode_instruction_t;

typedef struct {
	unsigned int pc;
	unsigned int clk;
	uint32_t reg_file[32];
	char cir[40];
	decode_instruction_t decoded;
	unsigned int cmp_reg;
	unsigned int halt_reg;
}cpu_t;

typedef enum {
	EQ = 1 << 0,
	GT = 1 << 1,
	LT = 1 << 2
}comparison_t;

#endif /* CPU_TYPES_H_ */

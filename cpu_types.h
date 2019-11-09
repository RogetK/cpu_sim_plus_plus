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
	MAX_MODES,
} cpu_loader_mode_t;

const char *mode_strings[] = {
		"data",
		"text",
};


typedef enum {
	r0,
	r1,
	r2,
	r3,
	MAX_OPREG,
} opreg_t;

typedef enum {
	NOP = 0,
	add,
	addi,
	MAX_INSTRUCTIONS,
}instructions_t;

typedef struct {
	instructions_t opcode;
	opreg_t src0;
	opreg_t src1;
	opreg_t src2;
} decode_instruction_t;

typedef struct {
	unsigned int pc;
	unsigned int clk;
	int reg_file[32];
	char cir[40];
	decode_instruction_t decoded;
}cpu_t;






#endif /* CPU_TYPES_H_ */

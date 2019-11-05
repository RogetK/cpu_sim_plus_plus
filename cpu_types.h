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


typedef struct {
	int pc;
	int clk;
	int reg_file[32];
	char cir[40];
}cpu_t;






#endif /* CPU_TYPES_H_ */

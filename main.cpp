#include <iostream>
#include <iterator>
#include <map>
#include "cpu_types.h"
#include <vector>
#include <fstream>
#include <string.h>

using namespace std;

#define REG_DST cpu.reg_file[cpu.decoded.src0]
#define REG_SRC1 cpu.reg_file[cpu.decoded.src1]
#define REG_SRC2 cpu.reg_file[cpu.decoded.src2]


// Variable declaration
char *iram[128];
uint32_t dram[128];
std::map<std::string, int> label_map;
cpu_t cpu;
unsigned int iram_size = 0;

/*** FUNCTION DECLARATION ***/

void loader(char *input_file) {
	ifstream in(input_file);
	char *assembly = new char [40];
//	assembly = (char *) calloc(40, sizeof(char*));

	int mode = no_mode;
	volatile int iram_loc = 0;
	volatile int dram_loc = 0;
	int ram_loc = 0;

	while(in){
		in.getline(assembly, 40);

		char *label;

		switch(assembly[0]) {
		case '.':
			for (int i = 0; i < MAX_MODES; i++) {
				if (!strcmp(assembly+1, mode_strings[i])){
					mode = (cpu_loader_mode_t) i;
				}
			}
			break;

		case ':':
			if (mode == text) {
				ram_loc = iram_loc;
			} else if (mode == data){
				ram_loc = dram_loc;
			}
			if (assembly[strlen(assembly)] == '\n'){
				assembly[strlen(assembly)] = '\0';
			}

			label = strtok(assembly, " \n");
			label_map.insert(std::make_pair(label, ram_loc));

			break;

		case ' ':
			assembly = assembly+3;
			/* no break */
		case '\t':
			if (mode == text) {
				iram[iram_loc] = (char *) malloc(40);
				strcpy(iram[iram_loc], assembly);
				iram_size++;
				iram_loc++;
			} else if (mode == data) {
				dram[dram_loc] = atoi(assembly);
				dram_loc++;
			}
			break;

		case '\n':
			break;

		case ';':
			cout << "Comment: " << assembly+1 << endl;
			break;

		default:
			break;
		}
	}

	in.close();
	delete(assembly);
}

void fetch(){
	strcpy(cpu.cir[0], iram[cpu.pc]);
}

void decode(){
	if (cpu.cir[0][0] == 0) return;
	char buffer[40];
	strcpy(buffer, cpu.cir[0]);


	int arg_num = 0;
	char *instruction = strtok(buffer, " ");

	for (int i = 0; i < MAX_INSTRUCTIONS; ++i) {
		if (strcmp(instruction, inst_string[i]) == 0) {
			cpu.decoded.opcode = (instructions_t) i;
		}
	}

	arg_num = instruction_args.find(cpu.decoded.opcode)->second;

	switch (arg_num) {
	case 1:{
		char *arg0_string = strtok(NULL, " \n");
		if (arg0_string[0] == 'r') cpu.decoded.src0 = (opreg_t) atoi(arg0_string+1);
		else if (arg0_string[0] == ':') cpu.decoded.src0i = label_map.find(arg0_string)->second;
		break;
	}
	case 2:{
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r') cpu.decoded.src0 = (opreg_t) atoi(arg0_string+1);

		char *arg1_string = strtok(NULL, " \n");
		if (arg1_string[0] == 'r') cpu.decoded.src1 = (opreg_t) atoi(arg1_string+1);
		else if (arg1_string[0] == '#') cpu.decoded.src1i = atoi(arg1_string+1);
		else if (arg1_string[0] == ':') cpu.decoded.src1i = label_map.find(arg1_string)->second;
		break;
	}
	case 3:{
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r') cpu.decoded.src0 = (opreg_t) atoi(arg0_string+1);

		char *arg1_string = strtok(NULL, " ");
		if (arg1_string[0] == 'r') cpu.decoded.src1 = (opreg_t) atoi(arg1_string+1);

		char *arg2_string = strtok(NULL, " \n");
		if (arg2_string[0] == '#') cpu.decoded.src2i = atoi(arg2_string+1);
		else if (arg2_string[0] == 'r') cpu.decoded.src2 = (opreg_t) atoi(arg2_string+1);
		else if (arg2_string[0] == ':') cpu.decoded.src2i = label_map.find(arg2_string)->second;
		break;
	}

	}

}

void flush(){
	memset(cpu.cir, 0, 40);
	cpu.decoded.opcode = NOP;
	cpu.decoded.src0i = 0;
	cpu.decoded.src1i = 0;
	cpu.decoded.src2i = 0;
}

void execute(){

	uint32_t src0, src1;
	switch (cpu.decoded.opcode) {
	case NOP:
		// increase pc;
		cpu.pc+=1;
		break;

	case ADD:
		src0 = REG_SRC1;
		src1 = REG_SRC2;
		REG_DST = src0 + src1;
		cpu.pc+=1;
		break;

	case ADDI:
		src0 = REG_SRC1;
		src1 = cpu.decoded.src2i;
		REG_DST = src0 + src1;
		cpu.pc+=1;
		break;

	case SUB:
		src0 = REG_SRC1;
		src1 = REG_SRC2;
		REG_DST = src0 - src1;
		cpu.pc+=1;
		break;

	case SUBI:
		src0 = REG_SRC1;
		src1 = cpu.decoded.src2i;
		REG_DST = src0 - src1;
		cpu.pc+=1;
		break;

	case MULT:
		src0 = REG_SRC1;
		src1 = REG_SRC2;
		REG_DST = src0 * src1;
		cpu.pc+=1;
		break;

	case DIVD:
		src0 = REG_SRC1;
		src1 = REG_SRC2;
		REG_DST = src0 / src1;
		cpu.pc+=1;
		break;

	case LD:
		src0 = REG_SRC1;
		src1 = cpu.decoded.src2i;
		REG_DST = dram[src0+src1];
		cpu.pc+=1;
		break;

	case LDI:
		REG_DST = cpu.decoded.src1i;
		cpu.pc+=1;
		break;

	case STO:
		src0 =	REG_SRC1;
		src1 = 	cpu.decoded.src2i;
		dram[src0 + src1] = REG_DST;
		cpu.pc+=1;
		break;

	//TODO: BRANCH
	case CMP:
		src0 = cpu.reg_file[cpu.decoded.src0];
		src1 = cpu.reg_file[cpu.decoded.src1];
		if (src0 < src1) cpu.cmp_reg = (uint8_t) LT;
		else if (src0 > src1) cpu.cmp_reg = (uint8_t) GT;
		else if (src0 == src1) cpu.cmp_reg = (uint8_t) EQ;
		cpu.pc+=1;
		break;

	case BLT:
		if (cpu.cmp_reg == LT) {
			cpu.pc = cpu.decoded.src0i;
			flush();
		}
		else cpu.pc+=1;
		break;

	case BGT:
		if (cpu.cmp_reg == GT) {
			cpu.pc = cpu.decoded.src0i;
			flush();
		}
		else cpu.pc+=1;
		break;

	case BEQ:
		if (cpu.cmp_reg == EQ) {
			cpu.pc = cpu.decoded.src0i;
			flush();
		}
		else cpu.pc+=1;
		break;

	case HALT:
		cpu.halt_reg= 1;
		cpu.pc+=1;
		break;

	default:
		break;

	}

}

void write(){

}

void populate_args(){
	//TODO: INSTRUCTION NUMBER OF ARGUMENTS ASSIGNMENT
	instruction_args[NOP] 	= 0;
	instruction_args[ADD] 	= 3;
	instruction_args[ADDI] 	= 3;
	instruction_args[SUB] 	= 3;
	instruction_args[SUBI] 	= 3;
	instruction_args[MULT] 	= 3;
	instruction_args[DIVD] 	= 3;
	instruction_args[LD] 	= 3;
	instruction_args[LDI]	= 2;
	instruction_args[CMP]	= 2;
	instruction_args[STO]	= 3;
	instruction_args[BLT]	= 1;
	instruction_args[BGT]	= 1;
	instruction_args[BEQ]	= 1;
	instruction_args[HALT] 	= 0;

}

void pipeline() {
	execute();
	decode();
	fetch();
}

/****** MAIN *******/

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Incorrect number of arguments" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "*** Processor Start ***" << endl;
	}

	cpu.cmp_reg = 0;
	populate_args();
	cout << argv[1] << endl;
	loader(argv[1]);

	// PIPELINE
	while (cpu.pc < iram_size && cpu.halt_reg != 1) {

		pipeline();
		write();
		cpu.clk++;

	}



	/*** OUTPUT PRINTS ***/
	cout << "*** Processor End ***\n" << endl;
	cout << "Program counter: " << cpu.pc << endl;
	cout << "CPU cycles: " << cpu.clk << endl;
	cout << "HALT: " << cpu.halt_reg << endl;

	printf("\nCMP_REG: %d\n", cpu.cmp_reg);
	cout << "Register File:\n";
	for (int i = 0; i < MAX_OPREG; i++) {
		cout << i << ":\t" <<cpu.reg_file[i] << endl;
	}
	cout << "CMP: " << cpu.cmp_reg << endl;

 	cout << "\nInstructions:\n";
	for (int i = 0; i < 128; i++){
		cout << i << "\t";
		cout << dram[i] << '\t' << iram[i] << endl;
	}

	return EXIT_SUCCESS;
}

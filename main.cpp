#include <iostream>
#include <iterator>
#include <map>
#include "cpu_types.h"
#include <vector>
#include <fstream>
#include <string.h>

using namespace std;
int halt_flag = 0;

/*** FUNCTION DECLARATION ***/
void loader(char *input_file) {
	ifstream in(input_file);
	char *assembly = (char *) calloc(40, sizeof(char*));
	char *p = assembly;
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
	free(p);
}

void issue(int a){
	for (int i = 0; i < a; i++){
		cpu.reservation[i].opcode = cpu.decoded[i].opcode;
		cpu.reservation[i].src0 = cpu.decoded[i].src0;
		cpu.reservation[i].src1 = cpu.decoded[i].src1;
		cpu.reservation[i].src2 = cpu.decoded[i].src2;
		cpu.reservation[i].src0i = cpu.decoded[i].src0i;
		cpu.reservation[i].src1i = cpu.decoded[i].src1i;
		cpu.reservation[i].src2i = cpu.decoded[i].src2i;
	}
}


void fetch(int a){
	for (int i = 0; i < a; i++ ){
		if (cpu.pc > iram_size-1) {
			memset(cpu.cir[i], 0, 40);
			return;
		}
		strcpy(cpu.cir[i], iram[cpu.pc]);
		cpu.pc+=1;
	}
}

void decode(int a){
	if (cpu.cir[a][0] == 0) return;
	char buffer[40];
	strcpy(buffer, cpu.cir[a]);


	int arg_num = 0;
	char *instruction = strtok(buffer, " ");

	for (int i = 0; i < MAX_INSTRUCTIONS; ++i) {
		if (strcmp(instruction, inst_string[i]) == 0) {
			cpu.decoded[a].opcode = (instructions_t) i;
		}
	}

	arg_num = instruction_args.find(cpu.decoded[a].opcode)->second;

	switch (arg_num) {
	case 1:{
		char *arg0_string = strtok(NULL, " \n");
		if (arg0_string[0] == 'r') cpu.decoded[a].src0 = (opreg_t) atoi(arg0_string+1);
		else if (arg0_string[0] == ':') cpu.decoded[a].src0i = label_map.find(arg0_string)->second;
		break;
	}
	case 2:{
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r') cpu.decoded[a].src0 = (opreg_t) atoi(arg0_string+1);

		char *arg1_string = strtok(NULL, " \n");
		if (arg1_string[0] == 'r') cpu.decoded[a].src1 = (opreg_t) atoi(arg1_string+1);
		else if (arg1_string[0] == '#') cpu.decoded[a].src1i = atoi(arg1_string+1);
		else if (arg1_string[0] == ':') cpu.decoded[a].src1i = label_map.find(arg1_string)->second;
		break;
	}
	case 3:{
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r') cpu.decoded[a].src0 = (opreg_t) atoi(arg0_string+1);

		char *arg1_string = strtok(NULL, " ");
		if (arg1_string[0] == 'r') cpu.decoded[a].src1 = (opreg_t) atoi(arg1_string+1);

		char *arg2_string = strtok(NULL, " \n");
		if (arg2_string[0] == '#') cpu.decoded[a].src2i = atoi(arg2_string+1);
		else if (arg2_string[0] == 'r') cpu.decoded[a].src2 = (opreg_t) atoi(arg2_string+1);
		else if (arg2_string[0] == ':') cpu.decoded[a].src2i = label_map.find(arg2_string)->second;
		break;
	}

	}

}

void flush(int a){
	memset(cpu.cir[a], 0, 40);
	cpu.decoded[a].opcode = NOP;
	cpu.decoded[a].src0i = 0;
	cpu.decoded[a].src1i = 0;
	cpu.decoded[a].src2i = 0;
}

void clear_decoded(int a){
	cpu.decoded[a].opcode = NOP;
	cpu.decoded[a].src0i = 0;
	cpu.decoded[a].src1i = 0;
	cpu.decoded[a].src2i = 0;

}

void execute(int a){

	uint32_t src1, src2;
	switch (cpu.decoded[a].opcode) {
	case NOP:
		break;

	case ADD:
		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.reg_file[cpu.decoded[a].src2];
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = src1 + src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case ADDI:
		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.decoded[a].src2i;
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = src1 + src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case SUB:

		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.reg_file[cpu.decoded[a].src2];
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = src1 - src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case SUBI:

		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.decoded[a].src2i;
		cpu.writeback_reg[a].dest = cpu.decoded[0].src0;
		cpu.writeback_reg[a].output = src1 - src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case MULT:
		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.reg_file[cpu.decoded[a].src2];
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = src1 * src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case DIVD:
		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.reg_file[cpu.decoded[a].src2];
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = src1 / src2;
		cpu.writeback_reg[a].write = 1;
		break;

	case LD:
		src1 = cpu.reg_file[cpu.decoded[a].src1];
		src2 = cpu.decoded[a].src2i;
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output= dram[src1+src2];
		cpu.writeback_reg[a].write = 1;
		break;

	case LDI:
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = cpu.decoded[a].src1i;
		cpu.writeback_reg[a].write = 1;
		break;

	case STO:
		src1 =	cpu.reg_file[cpu.decoded[a].src1];
		src2 = 	cpu.decoded[a].src2i;
		dram[src1 + src2] = cpu.reg_file[cpu.decoded[a].src0];
		break;

	case CMP:
		src1 = cpu.reg_file[cpu.decoded[a].src0];
		src2 = cpu.reg_file[cpu.decoded[a].src1];
		if (src1 < src2) {
			cpu.writeback_reg[a].dest = cmp;
			cpu.writeback_reg[a].output = (uint32_t) LT;
			cpu.writeback_reg[a].write = 1;
		} else if (src1 > src2) {
			cpu.writeback_reg[a].dest = cmp;
			cpu.writeback_reg[a].output = (uint32_t) GT;
			cpu.writeback_reg[a].write = 1;
		} else if (src1 == src2) {
			cpu.writeback_reg[a].dest = cmp;
			cpu.writeback_reg[a].output = (uint32_t) EQ;
			cpu.writeback_reg[a].write = 1;
		}
		break;

	case BLT:
		if (cpu.reg_file[cmp] == LT) {
			cpu.pc = cpu.decoded[a].src0i;
			flush(a);
		}
		break;

	case BGT:
		if (cpu.reg_file[cmp] == GT) {
			cpu.pc = cpu.decoded[a].src0i;
			flush(a);
		}
		break;

	case BEQ:
		if (cpu.reg_file[cmp] == EQ) {
			cpu.pc = cpu.decoded[a].src0i;
			flush(a);
		}
		break;

	case MOV:
		cpu.writeback_reg[a].dest = cpu.decoded[a].src0;
		cpu.writeback_reg[a].output = cpu.reg_file[cpu.decoded[a].src1];
		cpu.writeback_reg[a].write = 1;
		break;

	case J:
		cpu.pc = cpu.decoded[a].src0i;
		flush(a);
		break;

	case HALT:
		cpu.writeback_reg[a].dest = halt;
		cpu.writeback_reg[a].output = 1;
		cpu.writeback_reg[a].write = 1;
		break;

	default:
		break;
	}

	if (cpu.reg_file[halt] != 1){
		cpu.instructions_executed++;
	}
}

void writeback(int a){
	for (int i = 0; i < a; i++){
		if (cpu.writeback_reg[i].write) {
			cpu.reg_file[cpu.writeback_reg[i].dest] = cpu.writeback_reg[i].output;
			cpu.writeback_reg[i].write = 0;
		}
	}
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
	instruction_args[MOV] 	= 2;
	instruction_args[J]		= 1;
}

void pipeline(int a) {
	writeback(4);
	for (int i = 0; i < a; i++) execute(i);
	issue(a);
	for (int i = 0; i < a; i++) decode(i);
	fetch(a);
}

/****** MAIN *******/

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Incorrect number of arguments" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "*** Processor Start ***" << endl;
	}

	cout << "Input file: " << argv[1] << endl;

	populate_args();
	loader(argv[1]);

	// PIPELINE
	while (cpu.reg_file[halt] != 1) {
		pipeline(4);
		cpu.clk++;
	}

	/*** OUTPUT PRINTS ***/
	cout << "*** Processor End ***\n" << endl;
	cout << "Program counter: " << cpu.pc << endl;
	cout << "CPU cycles: " << cpu.clk << endl;
	cout << "Number of instructions run: " << cpu.instructions_executed << endl;
	cout << "HALT: " << cpu.halt_reg << endl;

//	printf("\nCMP_REG: %d\n", cpu.reg_file[cmp]);
	cout << "Register File:\n";
	for (int i = 0; i < MAX_OPREG; i++) {
		cout << i << ":\t" <<cpu.reg_file[i] << endl;
	}
//	cout << "CMP: " << cpu.reg_file[cmp] << endl;

 	cout << "\nInstructions:\n";
	for (int i = 0; i < 128; i++){
		cout << i << "\t";
		cout << dram[i] << '\t' << iram[i] << endl;
	}

	return EXIT_SUCCESS;
}

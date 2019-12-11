#include <iostream>
#include <iterator>
#include <map>
#include "cpu_types.h"
#include <vector>
#include <fstream>
#include <string.h>

#define SCALAR 4

using namespace std;
int halt_flag = 0;

/*** FUNCTION DECLARATION ***/
void loader(char *input_file) {
	ifstream in(input_file);
	char *assembly = (char*) calloc(40, sizeof(char*));
	char *p = assembly;
	int mode = no_mode;
	volatile int iram_loc = 0;
	volatile int dram_loc = 0;
	int ram_loc = 0;

	while (in) {
		in.getline(assembly, 40);
		char *label;

		switch (assembly[0]) {
		case '.':
			for (int i = 0; i < MAX_MODES; i++) {
				if (!strcmp(assembly + 1, mode_strings[i])) {
					mode = (cpu_loader_mode_t) i;
				}
			}
			break;

		case ':':
			if (mode == text) {
				ram_loc = iram_loc;
			} else if (mode == data) {
				ram_loc = dram_loc;
			}
			if (assembly[strlen(assembly)] == '\n') {
				assembly[strlen(assembly)] = '\0';
			}

			label = strtok(assembly, " \n");
			label_map.insert(std::make_pair(label, ram_loc));

			break;

		case ' ':
			assembly = assembly + 3;
			/* no break */
		case '\t':
			if (mode == text) {
				iram[iram_loc] = (char*) malloc(40);
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
			cout << "Comment: " << assembly + 1 << endl;
			break;

		default:
			break;
		}
	}

	in.close();
	free(p);
}

void issue(int ins, int units) {
	int skip = 0;
	int a = 0;
	for (; a < ins; a++) {

		for (; (skip + a) < units; skip++){
			if (	cpu.rs[a+skip].needs_count[0] == 0
					&& cpu.rs[a+skip].needs_count[1] == 0
					&& cpu.rs[a+skip].needs_count[2] == 0){
				break;
			}
		}

		if (skip+a == units) {
			cout << "RAN OUT OF EU\n";
			break;
		}

		if (cpu.reg_lock[cpu.decoded[a].src0]){
			if(cpu.decoded[a].isReg[0]){

//				cout << "DEP0 FAIL " << cpu.decoded[a].opcode << " " << cpu.decoded[a].src0 << endl;
				cpu.rs[a+skip].needs[0] = (opreg_t) cpu.decoded[a].src0;
				cpu.rs[a+skip].needs_count[0] = cpu.reg_lock[cpu.decoded[a].src0];
			}
		}

		if (cpu.reg_lock[cpu.decoded[a].src1]) {
			if (cpu.decoded[a].isReg[1]){

//				cout << "DEP1 FAIL " << cpu.decoded[a].opcode << " " << cpu.decoded[a].src1 << endl;
				cpu.rs[a+skip].needs[1] = (opreg_t)cpu.decoded[a].src1;
				cpu.rs[a+skip].needs_count[1] = cpu.reg_lock[cpu.decoded[a].src1];
			}
		}

		if (cpu.reg_lock[cpu.decoded[a].src2]) {
			if(cpu.decoded[a].isReg[2]){

//				cout << "DEP2 FAIL " << cpu.decoded[a].opcode << " " << cpu.decoded[a].src2 << endl;
				cpu.rs[a+skip].needs[2] = (opreg_t)cpu.decoded[a].src2;
				cpu.rs[a+skip].needs_count[2] = cpu.reg_lock[cpu.decoded[a].src2];
			}
		}


		cpu.rs[a+skip].rs.opcode = cpu.decoded[a].opcode;
		cpu.rs[a+skip].rs.src0 = cpu.decoded[a].src0;
		cpu.rs[a+skip].rs.src1 = cpu.decoded[a].src1;
		cpu.rs[a+skip].rs.src2 = cpu.decoded[a].src2;

		switch (cpu.decoded[a].opcode) {
		case STO:
		case J:
		case BLT:
		case BGT:
		case BEQ:
		case NOP:
			break;

		case HALT:
			cpu.reg_lock[halt]++;
			break;

		case CMP:
			cpu.reg_lock[cmp]++;
			break;
		default:
			cpu.reg_lock[cpu.decoded[a].src0]++;
		}
	}
}

void fetch(int a) {
	for (int i = 0; i < a; i++) {
		if (cpu.pc > iram_size - 1) {
			memset(cpu.cir[i], 0, 40);
			return;
		}
		strcpy(cpu.cir[i], iram[cpu.pc]);
		cpu.pc += 1;
	}
}

void decode(int a) {
	if (cpu.cir[a][0] == 0)
		return;
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
	case 1: {
		char *arg0_string = strtok(NULL, " \n");
		if (arg0_string[0] == 'r') {
			cpu.decoded[a].src0 = atoi(arg0_string + 1);
			cpu.decoded[a].isReg[0] = true;
		} else if (arg0_string[0] == ':'){
			cpu.decoded[a].src0 = label_map.find(arg0_string)->second;
			cpu.decoded[a].isReg[0] = false;
		}
		break;
	}
	case 2: {
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r'){
			cpu.decoded[a].src0 = atoi(arg0_string + 1);
			cpu.decoded[a].isReg[0] = true;
		}
		char *arg1_string = strtok(NULL, " \n");
		if (arg1_string[0] == 'r'){
			cpu.decoded[a].src1 =  atoi(arg1_string + 1);
			cpu.decoded[a].isReg[1] = true;
		}
		else if (arg1_string[0] == '#'){
			cpu.decoded[a].src1 = atoi(arg1_string + 1);
			cpu.decoded[a].isReg[1] = false;
		}
		else if (arg1_string[0] == ':'){
			cpu.decoded[a].src1 = label_map.find(arg1_string)->second;
			cpu.decoded[a].isReg[1] = false;
		}
		break;
	}
	case 3: {
		char *arg0_string = strtok(NULL, " ");
		if (arg0_string[0] == 'r'){
			cpu.decoded[a].src0 = atoi(arg0_string + 1);
			cpu.decoded[a].isReg[0] = true;
		}
		char *arg1_string = strtok(NULL, " ");
		if (arg1_string[0] == 'r'){
			cpu.decoded[a].src1 = atoi(arg1_string + 1);
			cpu.decoded[a].isReg[1] = true;
		}
		char *arg2_string = strtok(NULL, " \n");
		if (arg2_string[0] == '#'){
			cpu.decoded[a].src2 = atoi(arg2_string + 1);
			cpu.decoded[a].isReg[2] = false;
		}
		else if (arg2_string[0] == 'r'){
			cpu.decoded[a].src2 =  atoi(arg2_string + 1);
			cpu.decoded[a].isReg[2] = true;
		}
		else if (arg2_string[0] == ':'){
			cpu.decoded[a].src2 = label_map.find(arg2_string)->second;
			cpu.decoded[a].isReg[2] = false;
		}
		break;
	}

	}

//	for (int i = a-1; i > 0; i--){
//		if (cpu.rs[a].rs.src0 == cpu.decoded[i].src1){
//			cpu.rs[a].needs[0] = cpu.decoded[i].src1;
//			cpu.rs[a].busy = 1;
//		} else if (cpu.rs[a].rs.src0 == cpu.decoded[i].src2){
//			cpu.rs[a].needs[1] = cpu.decoded[i].src2;
//			cpu.rs[a].busy = 1;
//		}
//	}
//	issue(a);
//	cpu.rs[a].rs.opcode = cpu.decoded[a].opcode;
//	cpu.rs[a].rs.src0 = cpu.decoded[a].src0;
//	cpu.rs[a].rs.src1 = cpu.decoded[a].src1;
//	cpu.rs[a].rs.src2 = cpu.decoded[a].src2;
//	cpu.rs[a].rs.src0i = cpu.decoded[a].src0i;
//	cpu.rs[a].rs.src1i = cpu.decoded[a].src1i;
//	cpu.rs[a].rs.src2i = cpu.decoded[a].src2i;

}

void flush(int i) {
	for (int a = 0; a < i; a++) {
		memset(cpu.cir[a], 0, 40);
		cpu.decoded[a].opcode = NOP;
		cpu.rs[a].rs.opcode = NOP;
	}
}

//void clear_decoded(int a) {
//	cpu.decoded[a].opcode = NOP;
//	cpu.decoded[a].src0i = 0;
//	cpu.decoded[a].src1i = 0;
//	cpu.decoded[a].src2i = 0;
//
//}
//
void execute(int a) {

	uint32_t src1, src2;
	switch (cpu.rs[a].rs.opcode) {
	case NOP:
		break;

	case ADD:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.reg_file[cpu.rs[a].rs.src2];
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = src1 + src2;
		cpu.wbr[a].write = 1;
		break;

	case ADDI:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.rs[a].rs.src2;
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = src1 + src2;
		cpu.wbr[a].write = 1;
		break;

	case SUB:

		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.reg_file[cpu.rs[a].rs.src2];
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = src1 - src2;
		cpu.wbr[a].write = 1;
		break;

	case SUBI:

		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.rs[a].rs.src2;
		cpu.wbr[a].dest = (opreg_t) cpu.rs[0].rs.src0;
		cpu.wbr[a].output = src1 - src2;
		cpu.wbr[a].write = 1;
		break;

	case MULT:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.reg_file[cpu.rs[a].rs.src2];
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = src1 * src2;
		cpu.wbr[a].write = 1;
		break;

	case DIVD:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.reg_file[cpu.rs[a].rs.src2];
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = src1 / src2;
		cpu.wbr[a].write = 1;
		break;

	case LD:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.rs[a].rs.src2;
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = dram[src1 + src2];
		cpu.wbr[a].write = 1;
		break;

	case LDI:
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = cpu.rs[a].rs.src1;
		cpu.wbr[a].write = 1;
		break;

	case STO:
		src1 = cpu.reg_file[cpu.rs[a].rs.src1];
		src2 = cpu.rs[a].rs.src2;
		dram[src1 + src2] = cpu.reg_file[cpu.rs[a].rs.src0];
		break;

	case CMP:
		src1 = cpu.reg_file[cpu.rs[a].rs.src0];
		src2 = cpu.reg_file[cpu.rs[a].rs.src1];
		if (src1 < src2) {
			cpu.wbr[a].dest = cmp;
			cpu.wbr[a].output = (uint32_t) LT;
			cpu.wbr[a].write = 1;
		} else if (src1 > src2) {
			cpu.wbr[a].dest = cmp;
			cpu.wbr[a].output = (uint32_t) GT;
			cpu.wbr[a].write = 1;
		} else if (src1 == src2) {
			cpu.wbr[a].dest = cmp;
			cpu.wbr[a].output = (uint32_t) EQ;
			cpu.wbr[a].write = 1;
		}
		break;

	case BLT:
		if (cpu.reg_file[cmp] == LT) {
			cout << "BLT\n";
			cpu.pc = cpu.rs[a].rs.src0;
			flush(SCALAR);
		}
		break;

	case BGT:
		if (cpu.reg_file[cmp] == GT) {
			cpu.pc = cpu.rs[a].rs.src0;
			flush(SCALAR);
		}
		break;

	case BEQ:
		if (cpu.reg_file[cmp] == EQ) {
			cpu.pc = cpu.rs[a].rs.src0;
			flush(SCALAR);
		}
		break;

	case MOV:
		cpu.wbr[a].dest = (opreg_t) cpu.rs[a].rs.src0;
		cpu.wbr[a].output = cpu.reg_file[cpu.rs[a].rs.src1];
		cpu.wbr[a].write = 1;
		break;

	case J:
		cpu.pc = cpu.rs[a].rs.src0;
		flush(SCALAR);
		break;

	case HALT:
		cpu.wbr[a].dest = halt;
		cpu.wbr[a].output = 1;
		cpu.wbr[a].write = 1;
		break;

	default:
		break;
	}

	if (cpu.reg_file[halt] != 1) {
		cpu.instructions_executed++;
	}
}

void writeback(int a) {
	for (int i = 0; i < a; i++) {
		if (cpu.wbr[i].write) {
			cpu.reg_file[cpu.wbr[i].dest] = cpu.wbr[i].output;
			cpu.wbr[i].write = 0;
			cpu.reg_lock[cpu.wbr[i].dest]--;
			for (int j = 0; j < a; j++){
				for (int k = 0; k < 3; k++){
					if(cpu.rs[j].needs[k] == cpu.wbr[i].dest && cpu.rs[j].needs_count[k] > 0){
						cpu.rs[j].needs_count[k]--;
					}
				}
			}
//			cout << "DECREMENT " << cpu.wbr[i].dest << endl;
		}
	}
}

void populate_args() {
	//TODO: INSTRUCTION NUMBER OF ARGUMENTS ASSIGNMENT
	instruction_args[NOP] = 0;
	instruction_args[ADD] = 3;
	instruction_args[ADDI] = 3;
	instruction_args[SUB] = 3;
	instruction_args[SUBI] = 3;
	instruction_args[MULT] = 3;
	instruction_args[DIVD] = 3;
	instruction_args[LD] = 3;
	instruction_args[LDI] = 2;
	instruction_args[CMP] = 2;
	instruction_args[STO] = 3;
	instruction_args[BLT] = 1;
	instruction_args[BGT] = 1;
	instruction_args[BEQ] = 1;
	instruction_args[HALT] = 0;
	instruction_args[MOV] = 2;
	instruction_args[J] = 1;
}

void pipeline(int a) {
	cout << "Tick " << cpu.pc << endl;

	writeback(a);

	for (int i = 0; i < a; i++) execute(i);

	issue (a-2, a);

	for (int i = 0; i < a-2; i++) decode(i);

	fetch(a-2);

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
		pipeline(SCALAR);
		cpu.clk++;
	}

	/*** OUTPUT PRINTS ***/
	cout << "*** Processor End ***\n" << endl;
	cout << "Program counter: " << cpu.pc << endl;
	cout << "CPU cycles: " << cpu.clk << endl;
	cout << "Number of instructions run: " << cpu.instructions_executed << endl;
	cout << "HALT: " << cpu.reg_file[halt] << endl;

//	printf("\nCMP_REG: %d\n", cpu.reg_file[cmp]);
	cout << "Register File:\n";
	for (int i = 0; i < MAX_OPREG; i++) {
		cout << i << ":\t" << cpu.reg_file[i];
		cout << "\t" << cpu.reg_lock[i] << endl;
	}
//	cout << "CMP: " << cpu.reg_file[cmp] << endl;

	cout << "\nInstructions:\n";
	for (int i = 0; i < 128; i++) {
		cout << i << "\t";
		cout << dram[i] << '\t' << iram[i] << endl;
	}

	return EXIT_SUCCESS;
}

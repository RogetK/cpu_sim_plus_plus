#include <iostream>
#include <iterator>
#include <map>
#include "cpu_types.h"
#include <vector>
#include <fstream>
#include <string.h>

using namespace std;

// Variable declaration
char *iram[128];
uint32_t dram[128];
std::map<std::string, int> label_map;
cpu_t cpu;

void loader(char *input_file) {
	ifstream in(input_file);
	char *assembly;
	assembly = (char *) calloc(40, sizeof(char*));

	int mode = no_mode;
	volatile int iram_loc = 0;
	volatile int dram_loc = 0;
	int ram_loc = 0;

	while(in){
		in.getline(assembly, 40);

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
			// TODO: Dictionary / Map
			label_map.insert(std::make_pair(assembly, ram_loc));
			break;

		case ' ':
			assembly = assembly+3;
			/* no break */
		case '\t':
			if (mode == text) {
				iram[iram_loc] = (char *) malloc(40);
				strcpy(iram[iram_loc], assembly);
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
}

void fetch(){

}

void decode(){

}

void execute(){

}

/****** MAIN *******/

int main(int argc, char **argv) {
	if (argc != 2) {
		cout << "Incorrect number of arguments" << endl;
		return EXIT_FAILURE;
	} else {
		cout << "*** Processor Start ***" << endl;
	}

	loader(argv[1]);


	// PIPELINE
	while (cpu.pc < 5) {

		fetch();

		cpu.pc++;
		cpu.clk++;
	}



	/*** OUTPUT PRINTS ***/
	cout << "*** Processor End ***\n" << endl;
	cout << "Program counter: " << cpu.pc << endl;
	cout << "CPU cycles: " << cpu.clk << endl;
	cout << "\nInstructions:\n";
	for (int i = 0; i < 5; i++){
		cout << dram[i] <<iram[i] << endl;
	}

	return 0;
}

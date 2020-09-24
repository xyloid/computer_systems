// Implement your cycle count tool here.
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a data structure to store result.
typedef struct instruction {
	// I admit here I could make it more flexible by using char* name.
	// But since the assignment only requires a small number of instructons,
	// I decided to avoid malloc since it's error prone and adds overhead.
	char name[5];
	unsigned int cycles;
	unsigned int count;
} Instr;

// Use an array of pointers, so swap can be done with copying a smaller size of
// memory.
void swap_ins(Instr **a, Instr **b) {
	Instr *temp = *a;
	*a = *b;
	*b = temp;
}

// Quick sort is used to sort the array of instructions. So I can use binary
// search when the program is iterating over all the instructions in the file.
// The folllowing two functions are almost the same with the funtions in cs5006
// lab6. The partition function is used to partition the array into two parts.
int partition(Instr **set, int low, int high) {
	int i = low - 1;
	int j;
	Instr *pivot = set[high];
	for (j = low; j < high; j++) {
		if (strcmp(set[j]->name, pivot->name) <= 0) {
			i++;
			swap_ins(&set[j], &set[i]);
		}
	}
	swap_ins(&set[i + 1], &set[high]);
	return i + 1;
}

// The quick sort function.
void quick_sort(Instr **set, int low, int high) {
	if (low < high) {
		int p = partition(set, low, high);
		quick_sort(set, low, p - 1);
		quick_sort(set, p + 1, high);
	}
}

// Function count_instruction:
// Arguments:
// 	set: an array pointers pointing to the instruction structs.
//	size: size of the array.
//	code: the string of instruction being checked.
//
// Does: if the given instruction is found in the given array, then the
// corresponding count will increment by 1.
void count_instruction(Instr **set, unsigned int size, const char *code) {
	// Buffer for substrings as ADDL, ADDQ are regarded as ADD.
	char three[4];
	char four[5];

	// Binary search is used for efficiency.
	int low = 0;
	int high = size - 1;
	int mid = 0;
	while (low <= high) {
		mid = (low + high) / 2;

		// Calculate the length of the current instruction in the instruction
		// array.
		int length = 0;
		while (set[mid]->name[length] != '\0') {
			length++;
		}
		// Get the correct section of the current instruction from the file.
		char *tmp = NULL;
		if (length == 3) {
			// '\0' is the sign of string end.
			memset(three, '\0', sizeof(three));
			strncpy(three, code, 3);
			// Convert all characters to upper case.
			int i = 0;
			for (i = 0; i < 3; i++) {
				three[i] = toupper(three[i]);
			}
			tmp = three;
		}
		if (length == 4) {
			// '\0' is the sign of string end.
			memset(four, '\0', sizeof(four));
			strncpy(four, code, 4);
			// Convert all characters to upper case.
			int i;
			for (i = 0; i < 4; i++) {
				four[i] = toupper(four[i]);
			}

			tmp = four;
		}
		int cmp = strcmp(tmp, set[mid]->name);
		if (cmp < 0) {
			high = mid - 1;
		} else if (cmp > 0) {
			low = mid + 1;
		} else {
			// Match! Increment the count.
			set[mid]->count++;
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	Instr *sets[9];
	// Khoury server's CPU is XEON E5 2630, which is Sandy Bridge.
	// Cycles are estimated according to the latencies when operands are
	// registers or immidiate constants.
	Instr add = {"ADD", 1, 0};   // Latency 1
	Instr sub = {"SUB", 1, 0};   // Latency 1
	Instr mul = {"MUL", 4, 0};   // Latency 4
	Instr div = {"DIV", 30, 0};  // Latency 30
	Instr mov = {"MOV", 1, 0};   // Latency 1
	Instr lea = {"LEA", 3, 0};   // Latency 3
	Instr pop = {"POP", 2, 0};   // Latency 2
	Instr ret = {"RET", 2, 0};   // Latency 2
	Instr push = {"PUSH", 3, 0}; // Latency 3

	sets[0] = &add;
	sets[1] = &sub;
	sets[2] = &mul;
	sets[3] = &div;
	sets[4] = &mov;
	sets[5] = &lea;
	sets[6] = &pop;
	sets[7] = &ret;
	sets[8] = &push;

	quick_sort(sets, 0, 8);

	// Check argument number.
	if (argc != 2) {
		printf("Invalid argument number!\n");
		exit(1);
	}
	// Try to open file.
	FILE *file;
	file = fopen(argv[1], "r");
	char line[200];
	char head[200];

	// Scan instructions.
	if (file) {
		// Extract each line of the file.
		while (fgets(line, sizeof(line), file) != NULL) {
			// Use fscan to extract the string of the first word in the string
			// of whole line.
			if (fscanf(file, "%s", head) != EOF) {
				count_instruction(sets, 9, head);
			}
		}
	}
	fclose(file);

	int instr_num = 0;
	int cycles = 0;

	// Print individual counts.
	int i = 0;
	for (i = 0; i < 9; i++) {
		if (sets[i]->count > 0) {
			printf("%5s (latency:%3d):\t%d\n", sets[i]->name, sets[i]->cycles,
			       sets[i]->count);
			instr_num += sets[i]->count;
			cycles += sets[i]->count * sets[i]->cycles;
		}
	}

	// Print summary.
	printf("Total Instructions = %d\n", instr_num);
	printf("Total Cycles = %d\n", cycles);
	return 0;
}

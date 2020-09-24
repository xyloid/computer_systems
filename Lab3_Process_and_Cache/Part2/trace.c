//
// Compile with: clang -lm trace.c -o trace
//
// Run with: ./trace
//
// This program generates a trace file(s) that represents
// addresses that get put into a cache.
//
//
#include "binary.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Toggle these values to generate different
// addresses
#define BITWIDTH 8     // Our architecture is an 8-bit architecture
#define ADDRESSES 1024 // We will generate 512 instructions
#define RANGE 256      // The range of these instructions is a value 0-32

// Generate a regular data access pattern
//
// params: 	size - How many addresses to generate
//		range - The range of addresses
void Trace(int size, int range) {
	FILE *fp = fopen("regular.txt", "w");

	for (int i = 0; i < size; i++) {
		char *bin = intToBinaryString((i % range), BITWIDTH);
		fprintf(fp, "%s\n", bin);
		free(bin);
	}

	fclose(fp);
}

// Generate a random data access pattern
void randomTrace(int size, int range) {
	FILE *fp = fopen("random.txt", "w");

	srand(time(NULL));

	for (int i = 0; i < size; i++) {
		char *bin = intToBinaryString((rand() % range), BITWIDTH);
		fprintf(fp, "%s\n", bin);
		free(bin);
	}

	fclose(fp);
}

// Generate a single value, over and over again.
// This is the most basic trace file.
void singleTrace(int size, int value) {
	FILE *fp = fopen("single.txt", "w");

	srand(time(NULL));

	for (int i = 0; i < size; i++) {
		char *bin = intToBinaryString(value, BITWIDTH);
		fprintf(fp, "%s\n", bin);
		free(bin);
	}

	fclose(fp);
}

// Generate a more realistic data access pattern
// This will generate addresses that may repeat over time
void realisticTrace(int size, int range) {
	FILE *fp = fopen("realistic.txt", "w");

	srand(time(NULL));

	for (int i = 0; i < size;) {

		int isJump = rand() % 2;
		// If a jump does not occur, then just generate a random instruction
		// otherwise, generate a few instructions and repeat them all several
		// times
		if (isJump == 0) {
			char *bin = intToBinaryString((rand() % range), BITWIDTH);
			fprintf(fp, "%s\n", bin);
			free(bin);
			++i;
		} else {

			char *bin1 = intToBinaryString((rand() % range), BITWIDTH);
			char *bin2 = intToBinaryString((rand() % range), BITWIDTH);
			char *bin3 = intToBinaryString((rand() % range), BITWIDTH);

			for (int j = 0; j < 10 && i < size; j++, i += 3) {
				fprintf(fp, "%s\n", bin1);
				fprintf(fp, "%s\n", bin2);
				fprintf(fp, "%s\n", bin3);
			}

			free(bin1);
			free(bin2);
			free(bin3);
		}
	}

	fclose(fp);
}

// Since we have 4 sets, at most we could have 4 cold misses. (As in
// simulator.c, block is currently unused.) Also, we will have conflict misses.
// Set 0: 0 4 8 ...
// Set 1: 1 5 9 ...
// Set 2: 2 6 10 ...
// Set 3: 3 7 11 ...
// We want generate an address that for each set, there will always be a new
// address.
void unfriendlyTrace(int size) {
	FILE *fp = fopen("unfriendly.txt", "w");

	for (int i = 0; i < size; i++) {
		// i%8, we have 0x00,0x01,0x02 ... 0x7. Each consecutive address with
		// same last 2-1 bits will have different last 0 bits. For example,
		// 0b00000110 is followed by 0b00000111. They belong to the same set in
		// the cache. So there will be a conflict miss.
		char *bin = intToBinaryString(i % 8, BITWIDTH);
		fprintf(fp, "%s\n", bin);
		free(bin);
	}
	fclose(fp);
}

// We have a very short program, we only have 4 lines and they form a loop.
// As we only have 4 lines, there won't be any capacity miss.
// As the 4 lines have different last 2 bits, there won't be any conflict
// misses. All we have are 4 cold misses.
void friendlyTrace(int size) {
	FILE *fp = fopen("friendly.txt", "w");
	for (int i = 0; i < size; i++) {
		// (i%4)*2 : multiply by 2 because the simulator use last 2-1 bits to
		// generate set index.
		char *bin = intToBinaryString((i % 4) * 2, BITWIDTH);
		fprintf(fp, "%s\n", bin);
		free(bin);
	}
	fclose(fp);
}

int main() {

	Trace(ADDRESSES, RANGE);
	randomTrace(ADDRESSES, RANGE);
	realisticTrace(ADDRESSES, RANGE);
	singleTrace(ADDRESSES, 42);
	friendlyTrace(ADDRESSES);
	unfriendlyTrace(ADDRESSES);

	// Some unit tests for the binary conversions if you want
	// to play around with larger sets.
	char *test1 = intToBinaryString(255, BITWIDTH);
	printf("Example: 255 = %s\n", test1);
	printf("Binary(0,0) of 255: %d\n", binaryStringRangeToInt(test1, 0, 0));
	printf("Binary(0,1) of 255: %d\n", binaryStringRangeToInt(test1, 0, 1));
	printf("Binary(0,2) of 255: %d\n", binaryStringRangeToInt(test1, 0, 2));
	printf("Binary(0,3) of 255: %d\n", binaryStringRangeToInt(test1, 0, 3));
	printf("Binary(0,4) of 255: %d\n", binaryStringRangeToInt(test1, 0, 4));
	printf("Binary(0,5) of 255: %d\n", binaryStringRangeToInt(test1, 0, 5));
	printf("Binary(0,6) of 255: %d\n", binaryStringRangeToInt(test1, 0, 6));
	printf("Binary(0,7) of 255: %d\n", binaryStringRangeToInt(test1, 0, 7));
	free(test1);

	char *test2 = intToBinaryString(0, BITWIDTH);
	printf("\nExample: 0 = %s\n", test2);
	printf("Binary(0,0) of 0: %d\n", binaryStringRangeToInt(test2, 0, 0));
	printf("Binary(1,1) of 0: %d\n", binaryStringRangeToInt(test2, 1, 1));
	printf("Binary(2,2) of 0: %d\n", binaryStringRangeToInt(test2, 2, 2));
	printf("Binary(3,3) of 0: %d\n", binaryStringRangeToInt(test2, 3, 3));
	printf("Binary(4,4) of 0: %d\n", binaryStringRangeToInt(test2, 4, 4));
	printf("Binary(5,5) of 0: %d\n", binaryStringRangeToInt(test2, 5, 5));
	printf("Binary(6,6) of 0: %d\n", binaryStringRangeToInt(test2, 6, 6));
	printf("Binary(7,7) of 0: %d\n", binaryStringRangeToInt(test2, 7, 7));
	free(test2);

	char *test3 = intToBinaryString(42, BITWIDTH);
	printf("\nExample: 42 = %s\n", test3);
	printf("Binary(0,0) of 42: %d\n", binaryStringRangeToInt(test3, 0, 0));
	printf("Binary(1,1) of 42: %d\n", binaryStringRangeToInt(test3, 1, 1));
	printf("Binary(2,2) of 42: %d\n", binaryStringRangeToInt(test3, 2, 2));
	printf("Binary(3,3) of 42: %d\n", binaryStringRangeToInt(test3, 3, 3));
	printf("Binary(4,4) of 42: %d\n", binaryStringRangeToInt(test3, 4, 4));
	printf("Binary(5,5) of 42: %d\n", binaryStringRangeToInt(test3, 5, 5));
	printf("Binary(6,6) of 42: %d\n", binaryStringRangeToInt(test3, 6, 6));
	printf("Binary(7,7) of 42: %d\n", binaryStringRangeToInt(test3, 7, 7));
	free(test3);

	char *test4 = intToBinaryString(81, BITWIDTH);
	printf("\nExample: 81 = %s\n", test4);
	printf("Binary(0,0) of 81: %d\n", binaryStringRangeToInt(test4, 0, 0));
	printf("Binary(0,1) of 81: %d\n", binaryStringRangeToInt(test4, 0, 1));
	printf("Binary(1,1) of 81: %d\n", binaryStringRangeToInt(test4, 1, 1));
	printf("Binary(1,2) of 81: %d\n", binaryStringRangeToInt(test4, 1, 2));
	printf("Binary(1,3) of 81: %d\n", binaryStringRangeToInt(test4, 1, 3));
	printf("Binary(2,5) of 81: %d\n", binaryStringRangeToInt(test4, 2, 5));
	printf("Binary(5,7) of 81: %d\n", binaryStringRangeToInt(test4, 5, 7));
	printf("Binary(6,7) of 81: %d\n", binaryStringRangeToInt(test4, 6, 7));
	printf("Binary(7,7) of 81: %d\n", binaryStringRangeToInt(test4, 7, 7));
	free(test4);

	return 0;
}

// Compile with: clang -std=c11 -lm simulator.c -o simulator
//
// Run with: ./simulator realistic.txt
//
// All in one go run with: clang -std=c11 -lm simulator.c -o simulator && ./simulator realistic.txt
//
// This program imports a trace file 
// and tells us how many cache hits and misses
// we get based on the policy functions you implement.
//
//
#include <stdio.h>
#include <stdlib.h>
#include "binary.h"
#include <string.h>
//#include <ncurses.h> // Not currently supported on ccs.neu.edu machines.

// Toggle these values to generate different
// addresses
#define BITWIDTH 8	// Our architecture is an 8-bit architecture. i.e. contains M= 2^m where m is BITWIDTH unique addresses.
#define ADDRESSES 1024	// By default we generate 1024 instructions
#define RANGE 256	// The range of these instructions is a value 0-256. Co-incides with 'M'

// =============== Our CPU ===============
// This is where all of our instructions are loaded up into
char instructionCache[ADDRESSES][BITWIDTH+1]; 
// This is our Program Counter which executes the next
// instruction. In this case, it looks at our instruction Cache.
// which for our (fictional) machine holds all the instructions
// that will be executed.
int PC = 0;				

// Attributes of our cache
#define E  1 // Number of cachelines. 1 cacheline in each of our 4 sets.
#define B  2 // Data block bits B = 2^b. Not currently used in simulator
#define S  4 // Number of cache sets. S=2^s. By default, start with 4.

// Some helper variables for the range of bits to collect to determine
// the cache set.
// Changing 'S' means you need to modify these.
const int indexStart = 1;
const int indexStop = 2;


char cacheLines[S][BITWIDTH+1];
int cacheOccupied[S];				// 1 or a 0 if occupied
// =============== Our CPU ===============


// =============== The Game ================
// Minimize the cache misses and maximize the cache hits!
int cacheHit = 0;
int cacheMisses = 0;

int costOfCacheHit = 1; // This is the latency
int costOfCacheMiss = 100; // This is latency,

int totalLatency =0;	// How fast or slow is our system
// =============== The Game ================


// This function loads a trace file that has been
// previously generated.
// Trace files consist of binary numbers which
// (fictionally) represent instructions that have
// been executed by a CPU.
void loadTrace(char* filename){
	FILE* fp = fopen(filename,"r");

	char str[BITWIDTH+2]; // Strings are null terminated, so there is one extra character, plus the endline
	int pos = 0;
	while(fgets(str,sizeof(str),fp)){
		str[strlen(str)-1]='\0';		// Remove the endline character by terminating it
		strcpy(instructionCache[pos],str);	// Allocates memory for our array
		//printf("%d: %s",pos, instructionCache[pos]); // print out lines read in from file
		++pos;
	}		

	fclose(fp);
}



// User interface showing our cache simulator each step along the way.
void drawCache(){

	printf("Hits: %d Misses: %d Total Latency: %d\n",cacheHit,cacheMisses,totalLatency);
	printf("Instruction[%d] is executed\n",binaryStringToInt(instructionCache[PC],BITWIDTH+1));
	
	printf("---------------------------\n");
	printf("valid | tag | index | value\n");
	printf("                %d     %s   \n", binaryStringRangeToInt(instructionCache[PC],indexStart,indexStop),
						 instructionCache[PC]);
	printf("---------------------------\n");

	for(int i =0; i < S; i++){
		printf("CacheLine: %s: \n",cacheLines[i]);
	}

	printf("Total Cache Hits: %d Cache Misses %d Total Latency: %d  \n",cacheHit,cacheMisses,totalLatency);

//	printf("\033[7A]");

}


// This function loads an address in the simulator.
// This is equivalent to moving the program counter.
// and then we decide if it goes in the cache or not.
void stepSimulator(){
	PC++;
}


// Scorekeeping update when we have a cache miss
void incrementCacheMiss(){
	// Any time we replace we up the count
	cacheMisses++;
	// Update the total latency
	totalLatency+=costOfCacheMiss;
}

// Scorekeeping update when we have a cache miss
void incrementCacheHit(){
	// Update our count
	cacheHit++;
	// Update our latency
	totalLatency+=costOfCacheHit;
}


// Decide which index should be replaced in our cache.
// And then perform the actual replacement.
void ReplacementPolicy(){
	// Figure out where to look in the cache based on the index.
	int index = binaryStringRangeToInt(instructionCache[PC],indexStart,indexStop);
	// The cache is now occupied
	cacheOccupied[index]=1;
	// Copy in the string to the cache
	strcpy(cacheLines[index],instructionCache[PC]);	// Allocates memory for our array
	// Any time we are replacing in our cache, that is a cache miss!
	incrementCacheMiss();	
}

// Decide whether an item is in the cache or not.
// If an item needs to be evicted, the replacement policy is called
// otherwise, we do not need to do any work.
void evictionPolicy(){	
	// Figure out where to look in the cache based on the index.
	int index = binaryStringRangeToInt(instructionCache[PC],indexStart,indexStop);

	// Great, if we have a cache Hit!
	if( strcmp(cacheLines[index],instructionCache[PC])==0){
		// Update our scoreboard.
		incrementCacheHit();
	}else{
		printf("Cache miss!\n");
		// Cache miss and decide who to keep.
		// In general, always swap in for a simple policy!	
		strcpy(cacheLines[index],instructionCache[PC]);	// Allocates memory for our array
		// Update our scoreboard.
		incrementCacheMiss();
	}

}

// check if cache is filled first.
// If it is filled, figure out whether something should
// be evicted and tally a cache hit or miss.
void checkCache(){
	// Figure out where to look in the cache based on the index.
	int index = binaryStringRangeToInt(instructionCache[PC],indexStart,indexStop);

	// If the cache is not occupied, it is a cache miss
	if(cacheOccupied[index]==0){
		printf("Cold Miss!\n");		
		ReplacementPolicy();
	}else if(cacheOccupied[index]==1){
		evictionPolicy();
	}

}


int main(int argc, char **argv){

	// Loads a programs execution. 
	// Ideally this would be happening one instruction at a time
	// However, for our purposes, we have a 'trace' of a program
	// That is, a captured execution of the addresses that were 
	// 'fetched' from the CPU.
	loadTrace(argv[1]);


	// Initiate our cache sets to be empty.
	// If there were additional blocks or cachelines in each set
	// we would also zero those out.
	// Remember, all programs start out empty.
	for(int i =0; i < S; ++i){
		cacheOccupied[i] = 0;
	}

	// Our simulator is interactive
	// While 'run' is set to 'y'
	// it will continue.
	char run='y';

	// We run our program until we run out of addresses to 
	// execute or the user terminates.
	while((run=='y'|| run=='a') && PC < ADDRESSES){
		printf("============================================================================\n\n");
		// (1) Check our cache and run our policy.
		checkCache();
		// (2) A visual display of our cache and our user interface
		drawCache();
		// (3) We execute one instruction (Increment the PC)
		stepSimulator();

		// Ask if we would like to continue stepping through
		// our simulation.
		if(run !='a'){
			printf("Would you like to continue? (y-yes | n-no | a-simulate to finish): ");
			scanf("%s",&run);
		}
	}

	// We end our simulation
	printf("Simulation Complete\n");
	return 0;
}

#ifndef BINARY_H
#define BINARY_H

#include <stdlib.h>
#include <math.h>
#include <string.h>

// This function converts a binary string to an integer
// Assume these are all unsigned addresses
int binaryStringToInt(char* bin, int size){
	int sum=0;

	for(int i =size-1; i > -1; --i){
		if(bin[i]=='1'){
			sum = sum + pow(2,size-1-i-1);
		}	
	}

	return sum;
}


// This function converts a binary string to an integer
// Assume these are all unsigned addresses
//
// This function can pick out a range of numbers
// That is, given the string "10000001"
//                            ^^^
//                            76543210 
//           A start of 5 and a stop of 7 returns the integer 4 (i.e. 100)
int binaryStringRangeToInt(char* bin, int start, int stop){
	int sum=0;
	int counter =0;
	unsigned long size = strlen(bin);

	for(int i = size-start-1; counter < stop-start+1; i--){
		if(bin[i]=='1'){	 
			sum = sum + pow(2,counter);
		}	
		counter++;
	}

	return sum;
}



// Convert Integer to char* string
// Takes an integer value and performs
// the division algorithm and forms a string.
//
// This function does malloc, so memory must be freed.
//
// TODO: Currently assuming we can handle only a maximum 64-bit addresses
// 	 Someday there will be 128-bit!
char* intToBinaryString(int i, int bits){
	// This representation assumes a little-endian representation
	// i.e. binary[0] would be the least significant bit
	//
	// The number six in binary is represented as
	// |63|62|61|..|3|2|1|0|
	//                1 0 0
	//                    ^binary[0]
	//                  ^binary[1]
	//                ^binary[2]  
	char* binary = (char*)malloc((bits+1)*sizeof(char));
	// Fill with zeros
	for(int i =0; i < bits; ++i){
		binary[i]='0';
	}
	binary[bits]='\0';	


	// Our starting value is i
	int value = i;
	int pos = bits-1; // bit we are setting, starting with the least-significant bit
	// ^ We do this so when we read the binary, left-to-right it matches the little-endian
	// representation.

	while(value > 0){
		
		if(value%2==0){
			binary[pos]='0';
		}
		else{
			binary[pos]='1';
		}
		value =	value / 2;
		pos--;
	}

	return binary;
}

#endif

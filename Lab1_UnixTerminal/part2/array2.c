// Write a C program called array2.c that has an array of 400 integers in the
// function of main that is dynamically allocated.
#include <stdlib.h>
int main() {
	int *data = (int *)malloc(sizeof(int) * 400);
	data[0] = 0;
	data[399] = 400;
	data[299] = 300;
	int data2[400];
	data2[0] = 20;
	data2[399] = 2400;
	return 0;
}

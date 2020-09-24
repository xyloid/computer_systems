#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	// Check argument number.
	if (argc != 2) {
		// Print error message to stderr.
		fprintf(stderr, "Invalid argument number!\n");
		// errno 22 invalid argument.
		return -22;
	}
	char *token = strtok(argv[1], " ");
	while (token != NULL) {
		printf("%s\n", token);
		token = strtok(NULL, " ");
	}
	return 0;
}

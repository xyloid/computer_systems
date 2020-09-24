#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {

	char *myargv[16]; // We can store up to 16 arguments.
	myargv[0] = "/bin/ls";
	myargv[1] = "-c";
	myargv[2] = "-l";

	myargv[3] = NULL;

	myargv[4] = "/bin/sleep";
	myargv[5] = "0.5";
	myargv[6] = NULL;

	myargv[7] = "/bin/ps";
	myargv[8] = NULL;

	// Test if environment variable is set.
	myargv[9] = "/bin/sh";
	myargv[10] = "-c";
	myargv[11] = "echo $TEST";
	myargv[12] = NULL;

	char *env[2];
	env[0] = "TEST=\"This is the test string.\"";
	env[1] = NULL;

	if (fork() == 0) {
		printf("Execute command 'ls -c -l'...\n");
		int errno = 0;
		errno = execve(myargv[0], myargv, NULL);
		printf("errno: %d\n", errno);
		printf("Child: Should never get here, as execve does not return on "
		       "success.\n");
		exit(1);
	} else {
		wait(NULL);
		printf("\tFirst command finishes.\n");
	}

	if (fork() == 0) {
		int errno = 0;
		printf("\tExecute command 'sleep 0.5'...\n");
		errno = execve(myargv[4], myargv + 4, NULL);
		printf("errno: %d\n", errno);
		printf("Child: Should never get here, as execve does not return on "
		       "success.\n");
		exit(1);
	} else {
		wait(NULL);
		printf("\tSecond command finishes.\n");
	}

	if (fork() == 0) {
		int errno = 0;
		printf("\tExecute command 'ps'...\n");
		errno = execve(myargv[7], myargv + 7, NULL);
		printf("errno: %d\n", errno);
		printf("Child: Should never get here, as execve does not return on "
		       "success.\n");
		exit(1);
	} else {
		wait(NULL);
		printf("\tThird command finishes.\n");
	}

	if (fork() == 0) {
		int errno = 0;
		printf("\tExecute command 'echo $TEST'...\n");
		errno = execve(myargv[9], myargv + 9, env);
		printf("errno: %d\n", errno);
		printf("Child: Should never get here, as execve does not return on "
		       "success.\n");
		exit(1);
	} else {
		wait(NULL);
		printf("\tFourth command finishes.\n");
	}
	return 0;
}

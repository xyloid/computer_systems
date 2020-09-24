/*
PLEASE IGNORE THIS FILE. IT'S ONLY USED TO EXPLORE PIPE BY MYSELF.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
	int fd_even_to_odd[2];
	int fd_odd_to_even[2];

	// Generate pipe;

	char *args[2];
	args[0] = "./dummy";
	args[1] = NULL;

	// Backup STDIN and STDOUT
	// int in_bk = dup(STDIN_FILENO);
	// int out_bk = dup(STDOUT_FILENO);

	int count = 0;

	int gd[2];
	int gdr[2];
	pipe(gd);

	printf("%d %d %d %d\n", gd[0], gd[1], gdr[0], gdr[1]);

	pipe(fd_even_to_odd);

	int pid = fork();

	if (pid == 0) {

		// Redirect STOUT to FD[1];
		// Close read end;
		close(fd_even_to_odd[0]);
		// Redirect write end;
		dup2(fd_even_to_odd[1], STDOUT_FILENO);
		// Close old fd[1];
		close(fd_even_to_odd[1]);

		// Now printf goes to FD[1], write end.
		execvp(args[0], args);
		return 0;
	} else {
		wait(NULL);

		// Initialize another pipe
		pipe(fd_odd_to_even);
		// Close write end in the parent;
		close(fd_even_to_odd[1]);
		// pipe(gdr);

		// Build pipe fdr for next to child to write

		printf("After0\n");
		count++;
	}

	pid = fork();
	if (pid == 0) {
		// Redirect STDIN to FD[0] read end;
		dup2(fd_even_to_odd[0], STDIN_FILENO);
		close(fd_even_to_odd[0]);
		close(fd_even_to_odd[1]);

		// Redirect STDOUT to write end.
		dup2(fd_odd_to_even[1], STDOUT_FILENO);
		close(fd_odd_to_even[0]);
		close(fd_odd_to_even[1]);

		execvp(args[0], args);
		return 0;
	} else {
		wait(NULL);
		close(fd_even_to_odd[0]);
		pipe(fd_even_to_odd);
		close(fd_odd_to_even[1]);
		// close(fd_even_to_odd[1]);
		printf("After child 1\n");
		count++;
	}

	printf("EO R: %d W: %d\n", fd_even_to_odd[0], fd_even_to_odd[1]);
	printf("OE R: %d W: %d\n", fd_odd_to_even[0], fd_odd_to_even[1]);
	// If close here, then chain broke.
	// close(fd_even_to_odd[0]);
	// pipe(fd_even_to_odd);
	printf("EO R: %d W: %d\n", fd_even_to_odd[0], fd_even_to_odd[1]);
	pid = fork();

	if (pid == 0) {
		// Redirect STDIN to FD[0] read end;
		dup2(fd_odd_to_even[0], STDIN_FILENO);
		// close(fd_odd_to_even[0]);
		close(fd_odd_to_even[1]);

		// Redirect STOUT to FD[1];
		// Close read end;
		close(fd_even_to_odd[0]);
		// Redirect write end;
		dup2(fd_even_to_odd[1], STDOUT_FILENO);
		// Close old fd[1];
		close(fd_even_to_odd[1]);

		execvp(args[0], args);
		return 0;
	} else {
		wait(NULL);
		// close(fd_odd_to_even[1]);
		close(fd_even_to_odd[1]);
		printf("After child 2\n");
		count++;
	}

	pid = fork();

	if (pid == 0) {
		dup2(fd_even_to_odd[0], STDIN_FILENO);
		close(fd_even_to_odd[0]);
		close(fd_even_to_odd[1]);
		execvp(args[0], args);
		return 0;
	} else {
		wait(NULL);
		// close(fd_even_to_odd[1]);
		printf("After child 3\n");
		count++;
	}
	char buf[80];
	fgets(buf,80,stdin);

	return 0;
}

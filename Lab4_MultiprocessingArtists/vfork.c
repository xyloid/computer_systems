// Implement your part 1 solution here
// gcc vfork.c -o vfork
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
char colors[64][64 * 3];
// Modify your paint function here
void paint(int workID) {
	printf("Artist %d is painting\n", workID);
	int r = workID << 2;
	int b = (64 - workID) * 4 - 1;
	int i;
	for (i = 0; i < 64; i++) {
		// Lower side is redder.
		colors[workID][i * 3] = r;
		// Right side is greener.
		colors[workID][i * 3 + 1] = i * 4;
		// Upper side is bluer.
		colors[workID][i * 3 + 2] = b;
	}
}

void save() {
	FILE *fp;
	fp = fopen("vfork.ppm", "w+");
	fputs("P3\n", fp);
	fputs("64 64\n", fp);
	fputs("255\n", fp);
	int i, j;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64 * 3; j++) {
			// If we use %d instead of %u, then the output range is [-127, 127]
			// instead of [0, 255]. Also, type cast is needed here.
			fprintf(fp, "%u",(unsigned char)colors[i][j]);
			fputs(" ", fp);
		}
		fputs("\n", fp);
	}
	fclose(fp);
}

int main(int argc, char **argv) {
	int *integers = malloc(sizeof(int) * 8000);

	int numberOfArtists = 64; // How many child processes do we want?

	pid_t pid;
	// main loop where we fork new threads
	int i;
	for (i = 0; i < numberOfArtists; i++) {
		// (1) Perform a fork
		pid = vfork();

		// (2) Make only the child do some work (i.e. paint) and then
		// terminate.
		if (pid == 0) {
			// TODO: make child paint
			paint(i);
			// if we use fork, we have to free the copied heap allocation.
			// free(integers);
			// TODO: Then terminate/exit child
			_exit(0);
		}
	}
	pid_t wpid;
	int status = 0;
	while ((wpid = wait(&status)) > 0)
		;

	save();
	printf("parent is exiting(last artist out!)\n");
	free(integers);
	return 0;
}

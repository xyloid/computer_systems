// Implement your part 2 solution here
// gcc -lpthread threads.c -o threads
#include <pthread.h>
#include <stdio.h>

#define NTHREADS 64

char colors[64][64 * 3];

void paint(int workID) {
	printf("Artist %d is painting\n", workID);
	int b = workID << 2;
	int r = (64 - workID) * 4 - 1;
	int i;
	for (i = 0; i < 64; i++) {
		// Upper side is redder.
		colors[workID][i * 3] = r;
		// Left side is greener.
		colors[workID][i * 3 + 1] = (64 - i) * 4 - 1;
		// Lower side is bluer.
		colors[workID][i * 3 + 2] = b;
	}
}

void *thread(void *vargp) {
	int i = *((int *)vargp);
	paint(i);
	return NULL;
}

void save() {
	FILE *fp;
	fp = fopen("threads.ppm", "w+");
	fputs("P3\n", fp);
	fputs("64 64\n", fp);
	fputs("255\n", fp);
	int i, j;
	for (i = 0; i < 64; i++) {
		for (j = 0; j < 64 * 3; j++) {
			// If we use %d instead of %u, then the output range is [-127, 127]
			// instead of [0, 255]. Also, type cast is needed here.
			fprintf(fp, "%u", (unsigned char)colors[i][j]);
			fputs(" ", fp);
		}
		fputs("\n", fp);
	}
	fclose(fp);
}

int main() {
	int temp[NTHREADS];
	pthread_t tids[NTHREADS];
	int i;
	for (i = 0; i < NTHREADS; i++) {
		// Copy the value of i into a buffer because the address of i will be
		// passed to each thread.
		temp[i] = i;
		pthread_create(&tids[i], NULL, thread, (void *)&temp[i]);
	}

	for (i = 0; i < NTHREADS; i++) {
		pthread_join(tids[i], NULL);
	}

	save();

	return 0;
}

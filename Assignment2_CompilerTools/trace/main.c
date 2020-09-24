#include <stdio.h>
#include <unistd.h>

void msg() {
	unsigned int t = 1;
	static unsigned int count = 0;
	printf("Hello! msg() call %u, Let me sleep %u seconds.\n", count++, t);
	sleep(t);
}

int main() {
	int i = 0;
	printf("Message fron main()\n");
	for (i = 0; i < 10; i++) {
		msg();
	}
	return 0;
}

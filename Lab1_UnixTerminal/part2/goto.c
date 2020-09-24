// Write a C program using the goto command and a label.
int main() {
	int a = 0;
	goto START;
	a += 2;
	a = a * a;
START:
	a++;
	if (a < 3)
		goto START;
	return 0;
}

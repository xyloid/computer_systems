// Write a C program that calls an add function(long add(long a, long b).
long add(long a, long b) {
	return a + b;
}

int main() {
	long a = 10;
	long b = 11;
	long c = add(a, b);
	return 0;
}

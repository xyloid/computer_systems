// Write a C program that swaps two integers in a function
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int main() {
	int x = 1;
	int y = 2;
	swap(&x, &y);
	return 0;
}

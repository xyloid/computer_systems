// Write a C program using a switch statement
int main() {
	int flag = 3;
	int value;
	switch (flag) {
	case 1:
		value = 11;
		break;
	case 3:
		value = 13;
		// Observe what happens if break is commented out.
		// break;
	case 5:
		value = 15;
		break;
	default:
		value = 99;
	}
	return 0;
}

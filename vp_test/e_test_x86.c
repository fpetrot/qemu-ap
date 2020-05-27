#include <stdlib.h>

int main(int argc, char *argv[]) {
	int i;
	double s, t, u;

	t = 1.0;
	s = 1.0;

	for (i = 1; i <= atoi(argv[1]); i++) {
		t *= i;
		u = 1.0;
		u /= t;
		s += u;
	}

	return 0;
}
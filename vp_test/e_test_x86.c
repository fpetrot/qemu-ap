int main(void) {
	int i;
	double s, t, u;

	t = 1.0;
	s = 1.0;

	for (i = 1; i <= 100; i++) {
		t *= i;
		u = 1.0;
		u /= t;
		s += u;
	}

	return 0;
}
#include <stdio.h>
#include "sincos.h"
int main(int v, char* c[])
{
	double x;
	double y;

	double f = 0;
	sincos(f, &x, &y);
	printf("at value 0: [%f]-[%f]\n", x, y);
	f = 3.1415;
	sincos(f, &x, &y);
	printf("at value 3.1415: [%f]-[%f]\n", x, y);
	return 0;
}

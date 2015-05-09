#include <stdio.h>
#include "iniaxis.hh"
int main(int argv, char* argc[])
{
	int v = iniAxis(0, argc[1]);
	printf("result: [%d]\n", v);
	return 0;
}

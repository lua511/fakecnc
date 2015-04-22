#include <stdio.h>
#include "_timer.h"

int main(int v, char* c[]){
	printf("clk_tck: [%f]\n", clk_tck());
	double et1 = etime();
	printf("etime: [%f]\n", et1);
	print_etime();
	esleep(1.5);
	print_etime();
	double et2 = etime();
	printf("etime: [%f]\n", et2);
	printf("etime diff: [%f]\n", et2 - et1);
	return 0;
}


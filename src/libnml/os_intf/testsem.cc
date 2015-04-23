#include <stdio.h>
#include <sys/ipc.h>
#include "sem.hh"
#include "_timer.h"

#define  SEMAPHORE_KEY 2

int main(int v, char* c[])
{
	RCS_SEMAPHORE 
			rcs(SEMAPHORE_KEY, RCS_SEMAPHORE_CREATE, 1.5, 0,1);
	printf("valid: [%d]\n", rcs.valid());

	//printf("flush: [%d]\n", rcs.flush());
	double tm1 = etime();
	printf("wait:[%d]\n",rcs.wait());
	double tm2 = etime();
	printf("time waited: [%f]\n", tm2 - tm1);

	//printf("flush: [%d]\n", rcs.flush());
	tm1 = etime();
	printf("wait:[%d]\n", rcs.wait());
	tm2 = etime();
	printf("time waited: [%f]\n", tm2 - tm1);

	rcs.post();
	//printf("flush: [%d]\n", rcs.flush());
	tm1 = etime();
	printf("wait:[%d]\n", rcs.wait());
	tm2 = etime();
	printf("time waited: [%f]\n", tm2 - tm1);
	return 0;
}

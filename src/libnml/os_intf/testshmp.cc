#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shm.hh"
#include "sem.hh"
#include "_timer.h"

#define MEM_SHARE_ID 998

int main(int c, char* v[])
{


	RCS_SHAREDMEM mem(MEM_SHARE_ID, 1024, RCS_SHAREDMEM_CREATE);
	printf("attached process: [%d]\n", mem.nattch());
	pid_t p = fork();
	if (p == 0)
	{
		printf("attached process: [%d]\n", mem.nattch());

	}
	else
	{
		wait(NULL);
		printf("ok,child has gone\n");
	}
	printf("bye\n");
	return 0;
}

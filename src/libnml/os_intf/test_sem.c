#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "_sem.h"
#include "_timer.h"

#define  KEY_V 1
int main(int v, char* c[])
{
	rcs_sem_t *prst = rcs_sem_open(KEY_V, IPC_CREAT, 0);
	double tm1 = etime();
	rcs_sem_wait(prst, 0.5);
	double tm2 = etime();
	printf("time wait: [%f]\n", tm2 - tm1);

	rcs_sem_post(prst);

	tm1 = etime();
	rcs_sem_wait(prst, 0.5);
	tm2 = etime();
	printf("time wait: [%f]\n", tm2 - tm1);

	rcs_sem_destroy(prst);
	rcs_sem_close(prst);
}

#include <stdio.h>
#include "_shm.h"

#define  MEM_IDKEY (1024 + 3)

int main(int v, char* c[])
{
	shm_t* t = rcs_shm_open(MEM_IDKEY, 128, 0);
	printf("[%s]\n", t == NULL ? "null" : "not null");
	rcs_shm_delete(t);

	return 0;
}

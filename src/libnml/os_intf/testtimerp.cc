#include <stdio.h>
#include "timer.hh"

int timercallback(void* arg)
{
	printf("i am timer callback\n");
	return 0;
}

int main(int v, char* c[])
{
	RCS_TIMER rt(0.5, (RCS_TIMERFUNC)NULL, (void*)NULL);
	rt.sync();
	
	printf("===begin sleep===\n");
	double dt1 = etime();
	rt.wait();
	double dt2 = etime();
	rt.wait();
	double dt3 = etime();
	printf("===end sleep===\n");
	printf("diff: [%f]\n", dt2 - dt1);
	printf("diff: [%f]\n", dt3 - dt2);
	// the doc says this will work.but actually,it not
	printf("***5 seconds***\n");
	dt1 = etime();
	RCS_TIMER* tm = new RCS_TIMER(5.0);
	while(0){
		tm->wait();
	}
	dt2 = etime();
	delete tm;
	printf("***[%f]***\n",dt2 - dt1);
	return 0;
}

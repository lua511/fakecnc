#include <stdio.h>
#include "notimplement.hh"

int callbackfun(const char* info)
{
	printf("%s\n", info);
	return 0;
}

int callbackwithexcption(const char* info)
{
	callbackfun(info);
	return 1;
}

int main(int v, char* c[])
{
	printf("===section 1===\n");
	ReportNotimplement::registerCallback(&callbackfun);

	ReportNotimplement::report("info with silence");

	printf("===section 2===\n");
	ReportNotimplement::registerCallback(&callbackwithexcption);

	ReportNotimplement::report("info with exception");

	printf("===over===\n");
	return 0;
}

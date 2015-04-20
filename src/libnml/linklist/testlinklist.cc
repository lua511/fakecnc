#include <stdio.h>
#include <string.h>

#include "linklist.hh"

#define  LINE_LEN 128

struct YouCopyIt
{
	char	info[LINE_LEN];
	int	v;
};
struct DonotCopy
{
	char	info2[LINE_LEN];
	int v;
	float fv;
};

int main(int argc, char* argv[])
{
	LinkedList lis;

	printf("==section a===\n");
	printf("is empty [%s]\n", lis.is_empty() ? "true" : "false");

	printf("===section b===\n");
	YouCopyIt yci;
	strcpy(yci.info, "hello you copy it");
	lis.store_at_head(&yci, sizeof(YouCopyIt), 1);

	YouCopyIt* pyci = (YouCopyIt*)lis.get_head();
	printf("the head info is [%s]\n", pyci->info);

	if (pyci == &yci)
	{
		printf("the date pointer is same\n");
	}
	else
	{
		printf("the date pointer is not same\n");
	}

	printf("===session c===\n");
	DonotCopy* pdc = new DonotCopy();
	strcpy(pdc->info2, "hello do not copy");
	lis.store_at_head(pdc, sizeof(DonotCopy), 0);
	DonotCopy* pdata = (DonotCopy*)lis.get_head();
	if (pdata == pdc)
	{
		printf("the date pointer is same\n");
	}
	else
	{
		printf("the date pointer is not same\n");
	}
	printf("that's all,byte\n");
	delete pdc;
}

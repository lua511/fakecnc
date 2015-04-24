#include "rcs_print.hh"

int main(int v, char* c[])
{
	set_rcs_print_destination(RCS_PRINT_TO_STDOUT);
	rcs_print("hello i am from [%s]\n", "rcs_print");
	return 0;
}

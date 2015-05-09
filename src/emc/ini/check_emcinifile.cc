#include <stdio.h>
#include "emcIniFile.hh"
int main(int v, char* c[])
{
	EmcIniFile eif;
	eif.Open(c[1]);
	EmcLinearUnits elu;
	ErrorCode ec = eif.FindLinearUnits(&elu, "LINEAR_UNITS", "TRAJ");
	printf("errorcode: [%d]\n", (int)ec);
	printf("linearunit: [%f]\n", elu);
	return 0;
}

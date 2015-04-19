#include "notimplement.hh"

ReportNotimplement::CallbackFun ReportNotimplement::callback = 0;

int ReportNotimplement::report(const char* info)
{
	if (callback)
	{
		if ((*callback)(info) == 1)
		{
			int eno = 0;
			return 1 / (1 * eno);
		}
	}
	return 0;
}

ReportNotimplement::CallbackFun 
  ReportNotimplement::registerCallback(ReportNotimplement::CallbackFun cb)
{
	callback = cb;
}

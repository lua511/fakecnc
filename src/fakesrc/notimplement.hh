#ifndef NOTIMPLEMENT_HH
#define NOTIMPLEMENT_HH

class ReportNotimplement
{
public:

	// 0: just report
	// 1: halt
	typedef int(*CallbackFun)(const char*);
private:
	
	static CallbackFun callback;

public:

	static CallbackFun registerCallback(CallbackFun cb);

	static int report(const char* info);
};


#endif

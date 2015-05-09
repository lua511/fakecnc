#ifndef _EMCINIFILE_HH_
#define _EMCINIFILE_HH_
 
#ifndef WINTER_RAW
#include "emc.hh"
#endif
#include "inifile.hh"

#ifdef WINTER_RAW
enum EmcAxisType {
	EMC_AXIS_LINEAR = 1,
	EMC_AXIS_ANGULAR = 2,
};
typedef double                  EmcLinearUnits;
typedef double                  EmcAngularUnits;
typedef IniFile::ErrorCode		ErrorCode;
#endif

class EmcIniFile : public IniFile {
public:
                                EmcIniFile(int errMask=0):IniFile(errMask){}

    ErrorCode                   Find(EmcAxisType *result, const char *tag,
                                     const char *section=NULL, int num = 1);
    ErrorCode                   Find(bool *result, const char *tag,
                                     const char *section, int num=1);
    ErrorCode                   FindLinearUnits(EmcLinearUnits *result,
                                                const char *tag,
                                                const char *section=NULL,
                                                int num=1);
    ErrorCode                   FindAngularUnits(EmcAngularUnits *result,
                                                 const char *tag,
                                                 const char *section=NULL,
                                                 int num=1);

    // From base class.
    ErrorCode                   Find(int *result, int min, int max,
                                     const char *tag,const char *section,
                                     int num=1){
                                    return(IniFile::Find(result, min, max,
                                                         tag, section, num));
                                }
    ErrorCode                   Find(int *result, const char *tag,
                                     const char *section=NULL, int num = 1){
                                    return(IniFile::Find(result,
                                                         tag, section, num));
                                }
    ErrorCode                   Find(double *result, double min, double max,
                                     const char *tag,const char *section,
                                     int num=1){
                                    return(IniFile::Find(result, min, max,
                                                         tag, section, num));
                                }
    ErrorCode                   Find(double *result, const char *tag,
                                     const char *section=NULL, int num = 1){
                                    return(IniFile::Find(result,
                                                         tag, section, num));
                                }
    const char *                Find(const char *tag, const char *section=NULL,
                                     int num = 1){
                                    return(IniFile::Find(tag, section, num));
                                }

private:
    static StrIntPair           axisTypeMap[];
    static StrIntPair           boolMap[];
    static StrDoublePair        linearUnitsMap[];
    static StrDoublePair        angularUnitsMap[];
};


#endif


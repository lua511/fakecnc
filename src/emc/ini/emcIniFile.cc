#include <math.h>           // M_PI.
#include "emcIniFile.hh"


IniFile::StrIntPair         EmcIniFile::axisTypeMap[] = {
    {"LINEAR", EMC_AXIS_LINEAR},
    {"ANGULAR", EMC_AXIS_ANGULAR},
    { NULL, 0 },
};

EmcIniFile::ErrorCode
EmcIniFile::Find(EmcAxisType *result,
                 const char *tag, const char *section, int num)
{
    return(IniFile::Find((int *)result, axisTypeMap, tag, section, num));
}


IniFile::StrIntPair         EmcIniFile::boolMap[] = {
    {"TRUE", 1},
    {"YES", 1},
    {"1", 1},
    {"FALSE", 0},
    {"NO", 0},
    {"0", 0},
    { NULL, 0 },
};

EmcIniFile::ErrorCode
EmcIniFile::Find(bool *result, const char *tag, const char *section, int num)
{
    ErrorCode               errCode;
    int                     value;

    if((errCode = IniFile::Find(&value, boolMap, tag,section,num)) == ERR_NONE){
        *result = (bool)value;
    }

    return(errCode);
}


// The next const struct holds pairs for linear units which are 
// valid under the [TRAJ] section. These are of the form {"name", value}.
// If the name "name" is encountered in the ini, the value will be used.
EmcIniFile::StrDoublePair   EmcIniFile::linearUnitsMap[] = {
    { "mm",         1.0 },
    { "metric",     1.0 },
    { "in",         1/25.4 },
    { "inch",       1/25.4 },
    { "imperial",   1/25.4 },
    { NULL,         0 },
};
    
EmcIniFile::ErrorCode
EmcIniFile::FindLinearUnits(EmcLinearUnits *result,
                 const char *tag, const char *section, int num)
{
    return(IniFile::Find((double *)result, linearUnitsMap, tag, section, num));
}


// The next const struct holds pairs for angular units which are 
// valid under the [TRAJ] section. These are of the form {"name", value}.
// If the name "name" is encountered in the ini, the value will be used.
EmcIniFile::StrDoublePair   EmcIniFile::angularUnitsMap[] = {
    { "deg",        1.0 },
    { "degree",     1.0 },
    { "grad",       0.9 },
    { "gon",        0.9 },
    { "rad",        M_PI / 180 },
    { "radian",     M_PI / 180 },
    { NULL,         0 },
};

EmcIniFile::ErrorCode
EmcIniFile::FindAngularUnits(EmcAngularUnits *result,
                 const char *tag, const char *section, int num)
{
    return(IniFile::Find((double *)result, angularUnitsMap, tag, section, num));
}


#ifndef PHANTOM_HH
#define PHANTOM_HH

#include "cms.hh"

class PHANTOMMEM:public CMS {
  public:
    PHANTOMMEM(const char *bufline, const char *procline);
      virtual ~ PHANTOMMEM();
    virtual CMS_STATUS main_access(void *_local, int *serial_number);
};

#endif


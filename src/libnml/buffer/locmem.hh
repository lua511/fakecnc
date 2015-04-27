#ifndef LOCMEM_HH
#define LOCMEM_HH

#include "cms.hh"		// class CMS
#include "linklist.hh"		// class LinkedList

struct BUFFERS_LIST_NODE {
    void *addr;
    long size;
    char name[64];
};

class LOCMEM:public CMS {
  public:
    LOCMEM(const char *bufline, const char *procline, int set_to_server =
	0, int set_to_master = 0);
      virtual ~ LOCMEM();
    CMS_STATUS main_access(void *_local, int *serial_number);

  protected:
    void *lm_addr;
    int buffer_id;
    BUFFERS_LIST_NODE *my_node;
    static LinkedList *buffers_list;
};

#endif


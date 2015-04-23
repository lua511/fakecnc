extern "C" {
#include <stdio.h>		/* NULL */
#include "_sem.h"
}
#include "sem.hh"
#ifndef WINTER_RAW
#include "rcs_print.hh"		// rcs_print_debug(),
#else
#include "os_intf_hold.hh"
#endif
				// PRINT_SEMAPHORE_ACTIVITY
#include "timer.hh"		// etime()
RCS_SEMAPHORE::RCS_SEMAPHORE(key_t _id, int _oflag, double _time,
    int _mode, int _state)
{
    /* save the constructor args */
    id = _id;
    oflag = _oflag;
    mode = _mode;
    state = _state;
    timeout = _time;

    if (oflag & RCS_SEMAPHORE_CREATE) {
	sem = rcs_sem_create(id, mode, state);
    } else {
	sem = rcs_sem_open(id, 0);
    }

    if (sem == NULL) {
	rcs_print_error
	    ("can't create semaphore (id = %lu, oflag = %d, timeout = %f, mode = 0x%X, state = %d)\n",
	    id, oflag, timeout, mode, state);
    }
}

int
  RCS_SEMAPHORE::valid()
{
    return (sem != NULL);
}

RCS_SEMAPHORE::~RCS_SEMAPHORE()
{
    if (sem == NULL)
	return;

    /* need to destroy the semaphore before closing our copy */
    if (oflag & RCS_SEMAPHORE_CREATE) {
	rcs_sem_destroy(sem);
    }
    rcs_sem_close(sem);
    sem = NULL;
}

int
  RCS_SEMAPHORE::wait()
{
    int retval;
    if (sem == NULL)
	return -1;
    retval = rcs_sem_wait(sem, timeout);
    return retval;
}

int RCS_SEMAPHORE::trywait()
{
    if (sem == NULL)
	return -1;
    return rcs_sem_trywait(sem);
}

int RCS_SEMAPHORE::post()
{
    if (sem == NULL)
	return -1;
    return rcs_sem_post(sem);
}

int RCS_SEMAPHORE::flush()
{
    if (sem == NULL)
	return -1;
    return rcs_sem_flush(sem);
}

int RCS_SEMAPHORE::setflag(int _oflag)
{
    oflag = _oflag;		/* we can reset whether this was the one who
				   created the semaphore */
    return (0);
}

int RCS_SEMAPHORE::clear()
{
    return rcs_sem_clear(sem);
}

// This constructor declared private to prevent copying.
RCS_SEMAPHORE::RCS_SEMAPHORE(RCS_SEMAPHORE & sem)
{
}


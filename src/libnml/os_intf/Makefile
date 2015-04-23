include ../../Makefile.inc

.PHONY: timer timerc shm shmc sem semc

BUILDIT = $(CXX) -o $@.out $^ -DWINTER_RAW os_intf_hold.cc
TIMERCSRC := _timer.c testtimerc.c
TIMERSRC := _timer.c timer.cc testtimerp.cc
SEMCSRC := _sem.c _timer.c test_sem.c
SEMSRC := _sem.c sem.cc testsem.cc _timer.c
SHMCSRC := _shm.c testshmc.c
SHMSRC := shm.cc testshmp.cc _shm.c _timer.c

clean:
	rm -rf *.out

timerc: $(TIMERCSRC)
	$(BUILDIT)
timer: $(TIMERSRC)
	$(BUILDIT)
semc: $(SEMCSRC)
	$(BUILDIT)
sem: $(SEMSRC)
	$(BUILDIT)
shmc: $(SHMCSRC)
	$(BUILDIT)
shm: $(SHMSRC)
	$(BUILDIT)

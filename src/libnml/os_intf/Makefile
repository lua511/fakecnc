include ../../Makefile.inc

.PHONY: ctimer tsem

ctimer:ctimer.out

ctimer.out: _timer.c testtimerc.c
	$(CXX) -o $@ $^ -DWINTER_RAW

clean:
	rm -rf *.out

tsem: tsem.out 
tsem.out: _sem.c _timer.c test_sem.c
	$(CXX) -o $@ $^ -DWINTER_RAW

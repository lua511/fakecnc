include ../../Makefile.inc

.PHONY: ctimer tsem ttsem cpptimer

ctimer:ctimer.out

ctimer.out: _timer.c testtimerc.c
	$(CXX) -o $@ $^ -DWINTER_RAW

clean:
	rm -rf *.out

tsem: tsem.out 
tsem.out: _sem.c _timer.c test_sem.c
	$(CXX) -o $@ $^ -DWINTER_RAW

ttsem: ttsem.out
ttsem.out: _sem.c sem.cc _timer.c testsem.cc
	$(CXX) -o $@ $^ -DWINTER_RAW -g

cpptimer: cpptimer.out

cpptimer.out: _sem.c sem.cc _timer.c timer.cc testtimerp.cc
	$(CXX) -o $@ $^ -DWINTER_RAW -g

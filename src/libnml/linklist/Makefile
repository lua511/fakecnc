include ../../Makefile.inc

.PHONY: test

SRCS := testlinklist.cc linklist.cc

test: testlinklist.out
testlinklist.out: $(SRCS)
	@$(CXX) -o $@ $(SRCS)

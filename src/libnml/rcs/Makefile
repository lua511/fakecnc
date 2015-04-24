include ../../Makefile.inc

.PHONY: rcsp clean

rcsp: rcs_print_test.cc rcs_print.cc ../linklist/linklist.cc
	$(CXX) -o $@.out $^ -I../linklist/ -g -DWINTER_RAW
clean:
	rm -rf *.out

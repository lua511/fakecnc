include ../../Makefile.inc

.PHONY: sin clean

sin: sincos.c check_sincos.c
	$(CXX) -o $@.out $^ -g -DWINTER_RAW
clean:
	rm -rf *.out

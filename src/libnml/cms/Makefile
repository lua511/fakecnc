include ../../Makefile.inc

.PHONY: tcp clean


tcp: check_tcp_opts.cc tcp_opts.cc 
	$(CXX) -o $@.out $^ -g -DWINTER_RAW ../os_intf/os_intf_hold.cc -I../os_intf/


clean:
	rm -rf *.out

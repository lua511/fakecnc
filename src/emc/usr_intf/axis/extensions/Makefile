include ../../../../Makefile.inc

.PHONY: pym clean

pym: emcmodule.cc
	$(CXX) -shared -o linuxcnc.so $^ -I/usr/include/python2.7
	mkdir -p $(LIB_DIR)/python
	cp linuxcnc.so $(LIB_DIR)/python/
clean:
	rm -rf *.so

include ../Makefile.inc

.PHONY: halp clean

halp: halmodule.cc
	$(CXX) -o _hal.so $^ -shared -I$(PYTHON_INCLUDE)
	cp _hal.so $(LIB_DIR)/python/
clean:
	rm -rf *.so
	rm -rf $(LIB_DIR)/python/_hal.so

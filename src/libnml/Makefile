include ../Makefile.inc

all: $(LIB_DIR)/nml.a

$(LIB_DIR)/nml.a: nml.a
	cp $^ $@
nml.a: nml.o
	$(AR) cr nml.a nml.o
nml.o: ./nml/nml.cc
	@$(CXX) -o $@ -I$(INCLUDE_DIR) $^

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf $(LIB_DIR)/nml.a

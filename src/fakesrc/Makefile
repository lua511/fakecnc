include ../Makefile.inc

all: $(LIB_DIR)/notimpl.a
$(LIB_DIR)/notimpl.a : notimpl.a
	cp $^ $@
notimpl.a : notimplement.o
	$(AR) cr $@ $^
notimpl.o : notimplement.cc
	$(CXX) -c -o $@ $^ -I$(INCLUDE_DIR)

clean:
	rm -rf *.o
	rm -rf *.a
	rm -rf $(LIB/DIR)/notimpl.a
	rm -rf testreport

check: testreport.cc
	@$(CXX) -o testreport -g $^ -I$(INCLUDE_DIR) $(LIB_DIR)/notimpl.a


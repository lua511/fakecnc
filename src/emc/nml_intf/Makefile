include ../../Makefile.inc

all: $(LIB_DIR)/linuxcnc.a

$(LIB_DIR)/linuxcnc.a: linuxcnc.a
	cp linuxcnc.a $@
linuxcnc.a: emcglb.o
	$(AR) rcs linuxcnc.a emcglb.o

emcglb.o: emcglb.c
	$(CXX) -c -o emcglb.o $(IFLAG) emcglb.c

clean:
	@rm -rf linuxcnc.a
	@rm -rf $(LIB_DIR)/linuxcnc.a
	@rm -rf *.o

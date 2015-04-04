include ../../Makefile.inc

CFLAG = -O2 -W -Wall 

all: $(LIB_DIR)/linuxcnc.a

$(LIB_DIR)/linuxcnc.a: linuxcnc.a
	cp linuxcnc.a $@
linuxcnc.a: emcglb.o
	$(AR) cr linuxcnc.a emcglb.o

emcglb.o: emcglb.c
	$(CXX) -c -o emcglb.o emcglb.c -I$(INCLUDE_DIR)

clean:
	@rm -rf linuxcnc.a
	@rm -rf $(LIB_DIR)/linuxcnc.a
	@rm -rf *.o

TESTSRC := \
	emc/nml_intf/emcglb.c \




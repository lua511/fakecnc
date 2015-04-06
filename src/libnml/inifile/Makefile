include ../../Makefile.inc

.PHONY: all lib clean

MYPATH := $(SRC_DIR)/libnml/inifile

BINVAR := inivar
LIBVAR := linuxcncini.a

IHEAD := -I$(INCLUDE_DIR)

all: lib $(BIN_DIR)/$(BINVAR)

$(BIN_DIR)/$(BINVAR) : $(BINVAR)
	@cp $< $@

$(BINVAR): inivar.c lib
	$(CXX) -o $@ $(IHEAD) $< $(LIB_DIR)/$(LIBVAR)

lib: $(LIB_DIR)/$(LIBVAR)

$(LIB_DIR)/$(LIBVAR): $(LIBVAR)
	@cp $< $@
	
$(LIBVAR): inifile.o
	@$(AR) -cr linuxcncini.a $<
inifile.o: inifile.cc
	@$(CXX) -c -o inifile.o $(IHEAD) $<

clean:
	rm -rf $(BINVAR)
	rm -rf $(BIN_DIR)/$(BINVAR)
	rm -rf $(LIB_DIR)/$(LIBVAR)
	rm -rf $(LIBVAR)

test: testinifile.c
	@$(CXX) -o testini $(IHEAD) $< $(LIB_DIR)/$(LIBVAR)

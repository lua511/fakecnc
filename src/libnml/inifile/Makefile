include ../../Makefile.inc

.PHONY: all lib clean

MYPATH := $(SRC_DIR)/libnml/inifile

BINVAR := inivar
LIBVAR := liblinuxcncini.so

IHEAD := -I$(MYPATH) -I$(SRC_DIR)

all: $(LIB_DIR)/$(LIBVAR).0 $(BIN_DIR)/$(BINVAR)
	echo $(BIN_DIR)/$(BINVAR)

$(BIN_DIR)/$(BINVAR) : $(BINVAR)
	@cp $(BINVAR) $(BIN_DIR)/

$(BINVAR): inivar.c
	$(CXX) -o $(BINVAR) $(IHEAD) $(LINKSO) inivar.c
	cp $(BINVAR) $(BIN_DIR)/

lib:$(LIB_DIR)/$(LIBVAR).0

$(LIB_DIR)/$(LIBVAR).0: $(LIBVAR)
	@cp $(LIBVAR) $(LIB_DIR)/$(LIBVAR).0
	
$(LIBVAR): inifile.cc
	@$(CXX) -shared -o $(LIBVAR) $(IHEAD) inifile.cc

clean:
	rm -rf $(BINVAR)
	rm -rf $(BIN_DIR)/$(BINVAR)
	rm -rf $(LIB_DIR)/$(LIBVAR).0
	rm -rf $(LIBVAR)

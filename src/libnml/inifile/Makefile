include ../../Makefile.inc

BINTAR := inivar

all: inivar.c
	cc -o $(BINTAR) inivar.c
	cp $(BINTAR) $(BIN_DIR)/

clean:
	rm -rf $(BINTAR)
	rm -rf $(BIN_DIR)/$(BINTAR)

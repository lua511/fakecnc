include Makefile.inc

all clean: bindir fakedir
	(cd emc; make $@)
	(cd fakesrc; make $@)
	(cd libnml/inifile; make $@)
bindir: | ../bin/
../bin/:
	mkdir -p $@
fakedir: | ../fakebin/
../fakebin/:
	mkdir -p $@

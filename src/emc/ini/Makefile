include ../../Makefile.inc

.PHONY: inif clean inia

LIBINI := ../../libnml/inifile

inif: check_emcinifile.cc emcIniFile.cc
	$(CXX) -o $@.out $^ $(LIBINI)/inifile.cc -I$(LIBINI) -g -DWINTER_RAW

clean:
	rm -rf *.out

inia: check_iniaxis.cc iniaxis.cc emcIniFile.cc
	$(CXX) -o $@.out $^ $(LIBINI)/inifile.cc -I$(LIBINI) -g -DWINTER_RAW

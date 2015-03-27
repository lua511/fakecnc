include Makefile.inc

SUBDIRS := \
	emc/motion emc/task \
	fakesrc libnml/inifile \
	emc/usr_intf/testnothing \
	emc/usr_intf/Makefile \



all clean: bindir fakedir
	for dir in $(SUBDIRS); do (cd $$dir; make $@); done
#	(cd emc; make $@)
#	(cd fakesrc; make $@)
#	(cd libnml/inifile; make $@)
bindir: | $(BIN_DIR)
$(BIN_DIR):
	mkdir -p $@
fakedir: | $(FAKEBIN_DIR)
$(FAKEBIN_DIR):
	mkdir -p $@

include Makefile.inc

SUBDIRS := \
	emc/motion emc/task \
	fakesrc libnml/inifile emc_nml_intf\
	emc/usr_intf/testnothing \
	emc/usr_intf \


HEADERS := \
	config.h \
	emc/nml_intf/emcglb.h \
	libnml/inifile/inifile.hh \
	libnml/inifile/inifile.h \
	emc/nml_intf/emccfg.h \
	emc/motion/emcmotcfg.h \
	libnml/posemath/posemath.h \
	emc/nml_intf/emcpos.h \



HEADS := $(notdir $(HEADERS))

.PHONY: FORCE_CPI all clean

all: FORCE_CPI
	@test -d $(BIN_DIR) || \
		(echo "mkdir $(BIN_DIR)" &&  mkdir -p $(BIN_DIR))
	@test -d $(FAKEBIN_DIR) || \
		(echo "mkdir $(FAKEBIN_DIR)" && mkdir -p $(FAKEBIN_DIR))
	@test -d $(LIB_DIR) || \
		(echo "mkdir $(LIB_DIR)" && mkdir -p $(LIB_DIR))
	for dir in $(SUBDIRS); do (cd $$dir; make $@); done
clean:
	for dir in $(SUBDIRS); do (cd $$dir; make $@); done

FORCE_CPI:
	@test -d $(INCLUDE_DIR) || \
		(echo mkdir $(INCLUDE_DIR) ; mkdir -p $(INCLUDE_DIR))
	@$(foreach h,$(HEADERS),cp -f $(h) $(INCLUDE_DIR)/;)

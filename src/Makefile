include Makefile.inc

SUBDIRS := \
	emc/motion emc/task \
	fakesrc libnml/inifile emc_nml_intf\
	emc/usr_intf/testnothing \
	emc/usr_intf \


HEADERS := \
	config.h \
	emc/nml_intf/emcglb.h \
	emc/nml_intf/emccfg.h \
	emc/nml_intf/emc_nml.hh \
	emc/nml_intf/emc.hh \
	emc/nml_intf/debugflags.h \
	emc/nml_intf/motion_types.h \
	emc/nml_intf/canon.hh \
	emc/nml_intf/canon_position.hh \
	emc/nml_intf/emctool.h \
	emc/rs274ngc/rs274ngc.hh \
	emc/rs274ngc/interp_base.hh \
	libnml/cmd_msg.hh \
	libnml/nml.hh \
	libnml/buffer/physmem.hh \
	libnml/buffer/rem_msg.hh \
	libnml/nml/nml_oi.hh \
	libnml/linklist/linklist.hh \
	libnml/nml/nml_type.hh \
	libnml/nml/nmlmsg.hh \
	libnml/nml/stat_msg.hh \
	libnml/nml/nml_srv.hh \
	libnml/nml/nmldiag.hh \
	libnml/rcs/rcs.hh \
	libnml/rcs/rcs_print.hh \
	libnml/rcs/rcsversion.h \
	libnml/cms/cms.hh \
	libnml/cms/cms_cfg.hh \
	libnml/cms/cms_user.hh \
	libnml/cms/cms_srv.hh \
	libnml/cms/cmsdiag.hh \
	libnml/os_intf/timer.hh \
	libnml/inifile/inifile.hh \
	libnml/inifile/inifile.h \
	emc/motion/emcmotcfg.h \
	libnml/posemath/posemath.h \
	emc/nml_intf/emcpos.h \
	fakesrc/notimplement.hh \



HEADS := $(notdir $(HEADERS))

.PHONY: FORCE_CPI fcpi all clean

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
fcpi: FORCE_CPI
FORCE_CPI:
	@test -d $(INCLUDE_DIR) || \
		(echo mkdir $(INCLUDE_DIR) ; mkdir -p $(INCLUDE_DIR))
	@$(foreach h,$(HEADERS),cp -f $(h) $(INCLUDE_DIR)/;)

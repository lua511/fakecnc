include ../../Makefile.inc

KEYSTICK := $(BIN_DIR)/keystick

all: $(KEYSTICK)

$(KEYSTICK): keystick
	@cp keystick $(KEYSTICK)

keystick : keystick.cc
	@$(CXX) -o keystick $(IFLAG) -g keystick.cc $(LIB_DIR)/linuxcnc.a \
	$(LIB_DIR)/linuxcncini.a \
	-lncurses

clean:
	@rm -rf keystick
	@rm -rf $(KEYSTICK)

include ../../Makefile.inc

KEYSTICK := $(BIN_DIR)/keystick

all: $(KEYSTICK)

$(KEYSTICK): keystick
	@cp keystick $(KEYSTICK)

keystick : keystick.cc
	@$(CC) -o keystick $(IFLAG) keystick.cc $(LIB_DIR)/linuxcnc.a

clean:
	@rm -rf keystick
	@rm -rf $(KEYSTICK)

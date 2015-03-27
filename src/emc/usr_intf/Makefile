include ../../Makefile.inc

KEYSTICK := $(BIN_DIR)/keystick

all: $(KEYSTICK)

$(KEYSTICK): keystick
	@cp keystick $(KEYSTICK)

keystick : keystick.cc
	@$(CC) -o keystick keystick.cc

clean:
	@echo "this is clean"

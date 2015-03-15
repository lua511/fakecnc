include ../../Makefile.inc

SRCS := \
	motion.c linuxcncsvr.c io.c halcmd.c \


OBJS := $(patsubst %.c, %, $(SRCS))

BINOBJS := $(patsubst %.c,$(BIN_DIR)/%,$(SRCS))


all: OBJ2

OBJ1: $(OBJS)
	$(foreach i,$(OBJS),$(CC) -o $(i) $(i).c;)

OBJ2: OBJ1
	$(foreach i,$(OBJS),cp $i $(BIN_DIR)/$i;)

clean:
	@$(foreach i,$(OBJS),rm -rf $(i);)
	@$(foreach i,$(BINOBJS),rm -rf $(i);)

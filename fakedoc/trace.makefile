ifeq ("$(origin VV)", "command line")
	OLD_SHELL := $(SHELL)
	SHELL = $(warning Building $@$(if $<, (from $<))$(if $?, ($? newer)))$(OLD_SHELL)
endif
.PHONY: all clean

#trace.makefile:8: Building bar
#trace.makefile:17: Building bar3
#trace.makefile:14: Building bar2 (from bar3) (bar3 newer)


all: foo.o bar bar2

bar:
	@touch $@
clean:
	@rm -rf foo.o
	@rm -rf bar

bar2: bar3
	@touch $@

bar3:
	@touch $@

include ../../../Makefile.inc

MYPATH := $(shell pwd)

SRCFILE := $(MYPATH)/extensions/minigl.c
TOSRC := $(MYPATH)/extensions/_toglmodule.c

HEAD_FLAG := -I$(PYTHON_INCLUDE)

# -I$(GL_INCLUDE)

MINIGL = $(PYTHON_LIB)/minigl.so
TOGL = $(PYTHON_LIB)/_togl.so


all: $(MINIGL) $(TOGL)
$(MINIGL): minigl.so
	@cp minigl.so $(MINIGL)
minigl.so: minigl.o
	$(CC) -shared minigl.o -o minigl.so 

minigl.o:
	@$(CC) -c -o minigl.o $(HEAD_FLAG) $(SRCFILE)

$(TOGL): _togl.so
	@cp _togl.so $(TOGL)

_togl.so: _togl.o
	@$(CC) -shared _togl.o -o _togl.so

_togl.o: $(TOSRC)
	@$(CC) -c -o _togl.o $(HEAD_FLAG) -I$(SRC_DIR) -I$(TCL_INCLUDE) -L/us/X11R6/lib -lX11 -lGLU -lXmu  $(TOSRC)


clean:
	@rm -rf minigl.o
	@rm -rf minigl.so
	@rm -rf $(MINIGL)
	@rm -rf _togl.o
	@rm -rf _togl.so
	@rm -rf $(TOGL)

#usae
#	make -f origin.makefile W=hello

default:
	@echo "origin W"
	@echo $(origin W)
	@echo "origin CC"
	@echo $(origin CC)
	@echo "shell,default is : /bin/sh"
	@echo $(SHELL)

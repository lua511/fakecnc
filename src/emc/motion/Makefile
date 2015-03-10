
all: motion
	cc -o motion motion.c
	cp motion ../../../bin/
	cc -o linuxcncsvr linuxcncsvr.c
	cp linuxcncsvr ../../../bin/
	cc -o io io.c
	cp io ../../../bin/
	cc -o halcmd halcmd.c
	cp halcmd ../../../bin/
clean:
	rm -rf motion	
	rm -rf ../../../bin/linuxcncsvr
	rm -rf ../../../bin/motion
	rm -rf ../../../bin/iio
	rm -rf ../../../bin/halcmd

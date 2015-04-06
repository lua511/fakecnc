all:
	cc -o wish wish.c
	cp wish ../../fakebin/

clean:
	rm -rf wish
	rm -rf ../../fakebin/wish

TMPVAR := emc/nml_itf/wish.c
test:
	echo $(patsubst %.cc,objects/%.o,$(patsubst %.c,objects/%.o,$(TMPVAR)))

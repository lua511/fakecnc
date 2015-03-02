# an sample doc of diferent assignment operations.

BASE1 = base1
BASE2 =

A := $(BASE1)
B = $(BASE1)
C ?= $(BASE2)
D := $(BASE2)
BASE1 = fake
BASE2 = fake2


target:
	@echo "it's still base1,the value will be evaluated at assign time"
	@echo $(A)
	@echo "it's fake,the value will be evaluated at using time."
	@echo $(B)
	@echo "it's fake2x,while using,it's null,so search BASE2"
	@echo $(C)x
	@echo "it's x,evaluated at assign time"
	@echo $(D)x

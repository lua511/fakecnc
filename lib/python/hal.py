import _hal
from _hal import *

class component(_hal.component):
    def things(self): return _hal.component.whoami(self)


c = component('test')

print c.things()


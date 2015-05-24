
import sys,os

BASE = sys.path[0]
print BASE
if not BASE.endswith('fakesrc'):
    print "exception,please check correct path"
    sys.exit(1)

BASE = os.path.abspath(os.path.join(BASE,"../.."))
BASE = os.path.join(BASE,"lib","python")
sys.path.insert(0,BASE)
print "lib path:",BASE

import _hal
rv = _hal.halecho("ls -ahl | grep \".py\"")
print '===',rv,'==='
rv = _hal.halecho("pwd")
print '***',rv,'***'
print _hal.__name__


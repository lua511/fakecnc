import sys, os
BASE = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]), ".."))
#sys.path.insert(0, os.path.join(BASE, "lib", "python"))
sys.path.insert(0,os.path.abspath("/root/linuxcnc/lib/python"))

import vcpparse
from Tkinter import Tk


#print sys.path

vcpparse.filename='test.xml'
pyvcp0 = Tk()
pyvcp0.title("testaa")
vcpparse.create_vcp(compname='test',master=pyvcp0)

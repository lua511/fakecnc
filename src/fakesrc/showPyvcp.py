import sys, os

BASE = os.path.abspath("/root/linuxcnc")
sys.path.insert(0, os.path.join(BASE, "lib", "python"))

from Tkinter import Tk
import vcpparse

pyvcp0 = Tk()
pyvcp0.title('test123')
vcpparse.filename='test234'
pycomp=vcpparse.create_vcp(master=pyvcp0)

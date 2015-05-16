#!/usr/bin/env python

import sys,os
#argv[0] is the full bin-like path,here is $WORK_DIR/script
#so join .. means $WORK_DIR
BASE = os.path.abspath(os.path.join(os.path.dirname(sys.argv[0]),".."))
if BASE.endswith('emc'):
    BASE = os.path.abspath(os.path.join(BASE,"../.."))

# $BASE/lib/python
sys.path.insert(0,os.path.join(BASE,"lib","python"))
print sys.path

import vcpparse

filename="test.xml"
read_file(filename)


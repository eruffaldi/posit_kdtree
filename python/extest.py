#!/usr/bin/env python

import extest
import ctypes
import numpy as np

q = np.zeros((4,5),dtype=np.float32)
q = np.ascontiguousarray(q, dtype=np.float32) # not neeeded here but in general

for i in range(0,q.shape[0]):
	for j in range(0,q.shape[1]):
		q[i,j] = i*1000+j
t = extest.World("ciao")
p = q.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
print (p.__class__)
# UGLY
print (t.passdata(ctypes.cast(p, ctypes.c_void_p).value,int(q.shape[0]),int(q.shape[1])))

print (t.greet())
t.set("bom dia!")
print (t.greet())

t.many(['Good Morning', 'Buon giorno', 'Kali mera'])
print (t.greet())



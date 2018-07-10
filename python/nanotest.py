#!/usr/bin/env python

import nanoflanns
import numpy as np

def ndarray2ptr(a):
	import ctypes
	if a.dtype == np.float32:
		p = a.ctypes.data_as(ctypes.POINTER(ctypes.c_float))
	elif a.dtype == np.int32:
		p = a.ctypes.data_as(ctypes.POINTER(ctypes.c_int32))
	elif a.dtype == np.int64:
		p = a.ctypes.data_as(ctypes.POINTER(ctypes.c_int64))
	else:
		raise Exception("unsupported type, only float32 and int32 and int64")
		p = None
	return ctypes.cast(p, ctypes.c_void_p).value
def doknnsearch(index,qp,k):
	rb = np.zeros(k,dtype=np.int32)
	qp = np.array(qp).astype(np.float32)
	n = index.knnSearchx(qp.size,ndarray2ptr(qp),rb.size,ndarray2ptr(rb))
	return rb[0:n]

def main():
	xclass = nanoflanns.KUse

	data = np.zeros((128,3),dtype=np.float32)
	data[0,:] = (3,2,7)
	data[1,:] = (3,2,8)
	t = xclass(3,data.size,ndarray2ptr(data),10) # dim,n,data,maxleaf
	t.buildIndex()
	print (doknnsearch(t,(3,2,7),10))


if __name__ == '__main__':
	main()

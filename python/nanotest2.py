#!/usr/bin/env python

import nanoflanns2
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
	n = index.knnSearchx(k,ndarray2ptr(qp),ndarray2ptr(rb))
	return rb[0:n]

def doradiussearch(r,qp,nres):
	output = np.zeros(nres,dtype=np.int32)
	qp = np.array(qp).astype(np.float32)
	n = index.radiusSearchx(r,ndarray2ptr(qp),nres,ndarray2ptr(output))
	return rb[0:n]

def main():
	print(dir(nanoflanns2))
	xclass = nanoflanns2.kdtree_any_float
	print ("options",xclass.list())
	
	data = np.zeros((128,4),dtype=np.float32)
	data[0,:] = (3,2,7,8)
	data[1,:] = (3,2,8,9)
	print ("init")
	t = xclass("float")
	print (dir(t),t.__class__)
	print(t.name(),t.itemsize(),t.itemalign())
	print("")
	print ("build")
	t.buildx(ndarray2ptr(data),data.size,3,10) # data,rows,dim,maxleaf
	#print (doknnsearch(t,(3,2,7),10))
	#print (doradiussearch(t,5,(3,2,7),10))


if __name__ == '__main__':
	main()

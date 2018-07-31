#!/usr/bin/env python
from __future__ import print_function 
import nanoflanns2
import numpy as np
import ctypes

def aid(x):
    # This function returns the memory
    # block address of an array.
    return x.__array_interface__['data'][0]

def get_data_base(arr):
    """For a given NumPy array, find the base array
    that owns the actual data."""
    base = arr
    while isinstance(base.base, np.ndarray):
        base = base.base
    return base

def arrays_share_data(x, y):
    return get_data_base(x) is get_data_base(y)

def ndarray2ptr(a,dt):
	if a.dtype != dt:
		raise Exception("Expected " + dt)
	if not a.flags['C_CONTIGUOUS']:
		raise Exception("Expected C_CONTIGUOUS")
	if len(a.strides) == 2:
		if a.strides[0] == a.shape[1]*a.dtype.itemsize and a.strides[0] == 1:
			raise Exception("Expected row major and not " + a.ctypes.strides)
	v = a.ctypes.data_as(ctypes.c_void_p)#.value
	#print ("ndarray2ptr",a[0:3,:],a.dtype,a.shape,v,dir(v),"to",v.value,aid(aid))
	return v.value


def doknnsearch(index,qp,k):
	rt = np.int32 if index.indexsize() == 4  else np.int64
	rb = np.zeros(k,dtype=rt)
	qp = np.array(qp).astype(np.float32)
	n = index.knnSearchx(k,ndarray2ptr(qp,np.float32),ndarray2ptr(rb,rt))
	return rb[0:n]

def doradiussearch(index,r,qp,nres):
	rt = np.int32 if index.indexsize() == 4  else np.int64
	rb = np.zeros(nres,dtype=rt)
	qp = np.array(qp).astype(np.float32)
	n = index.radiusSearchx(r,ndarray2ptr(qp,np.float32),nres,ndarray2ptr(rb,rt))
	return rb[0:n]


def main():
	print(dir(nanoflanns2))
	xclass = nanoflanns2.kdtree_any_float
	allt = xclass.list()
	print ("options",allt)

	data = np.zeros((10,4),dtype=np.float32)
	print(data.flags['C_CONTIGUOUS'],data.dtype)
	data[0,:] = (3,2,7,8)
	data[1,:] = (3,2,8,9)
	#print ("init",data)
	for bt in allt:
		t = xclass(bt)
		#print (dir(t),t.__class__)
		#print(t.name(),t.itemsize(),t.itemalign())
		#print("")
		#print("indexsize",t.indexsize())
		#print ("build",data.shape[0])
		#print(t.buildnp(data,10)) # data,rows,dim,maxleaf
		print ("\n\nbt ------ " ,bt)
		print (" ",t.buildx(ndarray2ptr(data,np.float32),data.shape[0],data.shape[1],10)) # data,rows,dim,maxleaf
		print(" ",t.printStats())
		print (" ",doknnsearch(t,(3,2,7,8),10))
		print (" ",doradiussearch(t,5,(3,2,7,8),10))


if __name__ == '__main__':
	main()

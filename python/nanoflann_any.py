#!/usr/bin/env python
from __future__ import print_function 
import pynanoflann_any
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
		raise Exception("Expected dtype:" + dt + " got:" + a.dtype + " in ndarray2ptr")
	if not a.flags['C_CONTIGUOUS']:
		raise Exception("Expected C_CONTIGUOUS in ndarray2ptr")
	if len(a.strides) == 2:
		if a.strides[0] == a.shape[1]*a.dtype.itemsize and a.strides[0] == 1:
			raise Exception("Expected flat with row major. The stride is: " + a.ctypes.strides)
	v = a.ctypes.data_as(ctypes.c_void_p)#.value
	#print ("ndarray2ptr",a[0:3,:],a.dtype,a.shape,v,dir(v),"to",v.value,aid(aid))
	return v.value


def doknnsearch(index,qp,k):
	rt = np.int32 if index.indexsize() == 4  else np.int64
	rb = np.zeros(k,dtype=rt)
	qp = np.array(qp).astype(np.float32)
	n = index.knnSearchx(k,ndarray2ptr(qp,np.float32),ndarray2ptr(rb,rt))
	print("answer items",n,"over",k)
	return rb[0:n]

def doradiussearch(index,radius,qp,nres):
	rt = np.int32 if index.indexsize() == 4  else np.int64
	rb = np.zeros(nres,dtype=rt)
	qp = np.array(qp).astype(np.float32)
	n = index.radiusSearchx(radius,ndarray2ptr(qp,np.float32),nres,ndarray2ptr(rb,rt))
	print("answer items",n,"over",nres)
	return rb[0:n]


def main():
	import argparse

	parser = argparse.ArgumentParser(description='Process some integers.')
	parser.add_argument('--float',action="store_true")
	parser.add_argument('--type','-t',default="all")
	parser.add_argument('--points','-n',default=10,type=int)
	parser.add_argument('--dims','-d',default=4,type=int)
	parser.add_argument('--maxleaf',default=10,type=int)
	parser.add_argument('--info','-i',action="store_true")

	args = parser.parse_args()
	print("main")	
	print(dir(pynanoflann_any))
	xclass = pynanoflann_any.kdtree_any_float
	allt = xclass.list()
	if args.type == "query":
		print(allt)
		return
	elif args.type != "all":
		allt = [args.type]
	if args.info:
		import tabulate
		x=[]
		for bt in allt:
			t = xclass(bt)
			l = np.array((0,0,0),dtype=np.float64)
			t.limitsx(ndarray2ptr(l,np.float64))
			x.append(dict(name=bt,itemsize=t.itemsize(),itemalign=t.itemalign(),indexsize=t.indexsize(),selfname=t.name(),limits=l))

		print(tabulate.tabulate(x))
		return
	print ("variants:",allt)

	data = np.zeros((args.points,args.dims),dtype=np.float32)
	print(data.flags['C_CONTIGUOUS'],data.dtype)
	if args.dims == 4:
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
		print("indexsize",t.indexsize())
		print("building")
		bb = t.buildx(ndarray2ptr(data,np.float32),data.shape[0],data.shape[1],args.maxleaf,False)		
		print("build result",bb)
		print(" stats",t.printStats())
		if args.dims == 4:
			print (" knn:",doknnsearch(t,(3,2,7,8),10))
			print (" rs:",doradiussearch(t,5,(3,2,7,8),10))

		if args.float:
			print ("\n\nbt ------ float -> " ,bt)
			print("building")
			bb = t.buildx(ndarray2ptr(data,np.float32),data.shape[0],data.shape[1],args.maxleaf,True)		
			print("build result",bb)
			print(" ",t.printStats())
			if args.dims == 4:
				print (" ",doknnsearch(t,(3,2,7,8),10))
				print (" ",doradiussearch(t,5,(3,2,7,8),10))

if __name__ == '__main__':
	main()

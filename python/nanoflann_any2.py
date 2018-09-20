from __future__ import absolute_import,print_function
import numpy
import sklearn.preprocessing

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


def mainold():
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
        print (" ",t.buildx(ndarray2ptr(data,np.float32),data.shape[0],data.shape[1],10),False) # data,rows,dim,maxleaf
        print(" ",t.printStats())
        print (" ",doknnsearch(t,(3,2,7,8),10))
        print (" ",doradiussearch(t,5,(3,2,7,8),10))




class PositKDD:
    def __init__(self, metric, rest, from_float_tree = False):
        print ("\n\n","PositKDD",metric,rest)
        type = rest.get("type","float")
        if metric not in ('euclidean', 'hamming'):
            raise NotImplementedError("PositKDD does not support metric " + self._metric)
        self.name = 'PositKDD(type=%s,metric=%s,mode=%s)' % (type,metric,"normal" if not from_float_tree else "from float tree")
        self._type = type
        self._metric = metric
        self._unk = 10
        self.from_float_tree = from_float_tree
        xclass = nanoflanns2.kdtree_any_float
        pt = self._type + ("" if metric == "euclidean" else "_"+metric)
        t = xclass(pt)
        if t is None:
            raise NotImplementedError("unsupported class "+ pt + " only: " + " ".join(xclass.list())) 
        self._index = t

    def fit(self, X):
        X = X.astype(numpy.float32)
        #self._flann = pyflann.FLANN(target_precision=self._target_precision, algorithm='autotuned', log_level='info')
        #if self._metric == 'angular':
        #    X = sklearn.preprocessing.normalize(X, axis=1, norm='l2')
        #self._flann.build_index(X)
        self._index.buildx(ndarray2ptr(X,np.float32),X.shape[0],X.shape[1],self._unk,self.from_float_tree)

    #def set_query_arguments(self, search_k):
    #    self._search_k = search_k

    def query(self, v, k):
        if self._metric == 'angular':
            v = sklearn.preprocessing.normalize([v], axis=1, norm='l2')[0]

        rt = np.int32 if self._index.indexsize() == 4  else np.int64
        rb = np.zeros(k,dtype=rt)
        v = np.array(v).astype(np.float32)
        n = self._index.knnSearchx(k,ndarray2ptr(v,np.float32),ndarray2ptr(rb,rt))
        return rb[0:n]

def main():
    print("main")   
    print(dir(pynanoflann_any))
    xclass = pynanoflann_any.kdtree_any_float
    allt = xclass.list()

    for x in allt:
        print("creating",x)
        a = PositKDD("euclidean",x)
        data = np.zeros((10,4),dtype=np.float32)
        print(data.flags['C_CONTIGUOUS'],data.dtype)
        data[0,:] = (3,2,7,8)
        data[1,:] = (3,2,8,9)
        #print ("init",data)
        a.fit(data)
        print ("query",a.query((3,2,7,8),2))

if __name__ == '__main__':
    main()

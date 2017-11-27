#!/usr/bin/python
import sys
from collections import OrderedDict

def main():
	l = open(sys.argv[1],"r").read().split("-----\n")

	bt = OrderedDict()
	btt = OrderedDict()
	ks = OrderedDict()
	rs = OrderedDict()
	im = OrderedDict()

	wa = [bt,btt,ks,rs,im]

	for a in l:
		ll = a.split("\n")
		which = ll[0]
		w = {}
		for y in wa:
			s = y.get(which)
			if s is None:
				y[which] = []

		for z in ll[1:]:
			a = z.split(":",1)
			if a[0] == "building time":
				bt[which].append(float(a[1]))
			elif a[0].startswith("radiusSearch time"):
				rs[which].append(float(a[1]))
			elif a[0].startswith("bbox time"):
				btt[which].append(float(a[1]))
			elif a[0].startswith("knnSearch time"):
				ks[which].append(float(a[1]))
			elif a[0].startswith("index memory"):
				im[which].append(float(a[1]))				

	for i,a in enumerate(wa): # every dictionary
		for k in a.keys(): # every key in dictionary
			v = a[k] # value
			if len(v) > 0:
				a[k] = sum(v)/len(v)
			else:
				a[k] = 0
	print "\nBBox Time\n"
	for k,v in btt.iteritems():
		if k != "":
			print k,v

	print "\n\nBuilding Time\n"
	for k,v in bt.iteritems():
		if k != "":
			print k,v

	print "\n\nKNN Search\n"
	for k,v in ks.iteritems():
		if k != "":
			print k,v

	print "\n\nRadius Search\n"
	for k,v in rs.iteritems():
		if k != "":
			print k,v

	print "\n\nIndex Memory\n"
	for k,v in im.iteritems():
		if k != "":
			print k,v

if __name__ == '__main__':
	main()
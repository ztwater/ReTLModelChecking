import sys
import numpy as np 

l = (int) (sys.argv[1])
dim = []
with open('model.txt', 'r') as f:
	for i in range(l-1):
		mline = np.array([float(x) for x in f.readline().split(' ')])
		bline = np.array([float(x) for x in f.readline().split(' ')])
		mline = mline.reshape(len(bline), len(mline)//len(bline))
		print(f"M_{i} = {np.array2string(mline, separator=', ')}")
		print(f"b_{i} = {np.array2string(bline, separator=', ')}")
		if i == 0:
			dim.append(mline.shape[1])
		dim.append(len(bline))
print('M = [', end='')
for i in range(l-2):
	print(f"M_{i}, ", end='')
print(f"M_{l-2}]")

print('b = [', end='')
for i in range(l-2):
	print(f"b_{i}, ", end='')
print(f"b_{l-2}]")

for i in range(l-1):
	print(f"d_{i}, ", end='')
print(f"d_{l-1} = ", end='')
for i in range(l-1):
	print(f"{dim[i]}, ", end='')
print(f"{dim[l-1]}")

print(f"d = [", end='')
for i in range(l-1):
	print(f"d_{i}, ", end='')
print(f"d_{l-1}]")

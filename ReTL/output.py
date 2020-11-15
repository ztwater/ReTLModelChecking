import solver as s
import numpy as np
import time

start = time.time()
M_0 = [[ 6., -7.],
 [-7.,  1.],
 [ 7.,  5.],
 [-7.,  1.],
 [ 9., -6.]]
b_0 = [-10.,  -3.,   4.,   6.,   1.]
M_1 = [[ 5.,  4., -8.,  4., -8.],
 [-7., -3., -4., -1., -5.]]
b_1 = [-10.,  -1.]
M = [M_0, M_1]
b = [b_0, b_1]
d_0, d_1, d_2 = 2, 5, 2
d = [d_0, d_1, d_2]
var_list = []
all_list = []
xst_list = []
extra_inequalities = []
x_1 = s.symbols('x_1')
x_2 = s.symbols('x_2')
x = [x_1, x_2]
var_list += x
all_list += x
layer_0 = [13, 12, 7, 3, 2, 6, 5, 11, 10]
layer_1 = [8]
layer_2 = [9]
layers = [layer_0, layer_1, layer_2]
all_list, xst_list = xst_list, all_list
if len(all_list) == 0:
	res = []
else:
	res = [[s.Reals]*len(all_list)]
e1 = ['G', 'G']
e2 = [-0.010000, -0.010000]
e3 = s.get_inequalities(x, e1, e2)
e4 = ['L', 'L']
e5 = [0.010000, 0.010000]
e6 = s.get_inequalities(x, e4, e5)
e7 = s.AND(e3, e6)
e8 = s.circ(x, 0, layers, M, b)
for a_8 in range(s.get_dim(8, d, layers)):
	extra_inequalities = [e8[1][a_8]]
	e9 = s.circ(e8[0][a_8], 8, layers, M, b)
	e10 = [1, -1]
	e11 = [np.array([[s.vec_dot(e9, e10), '>', 0]], dtype=object)]
	e12 = s.OR(s.NOT(e7), e11)
	e13 = s.Forall(x, e12)
	e13 = s.unify(e13, 1)
	e13 = s.AND(e13, extra_inequalities)
	res = s.q_solve(e13, var_list, all_list, xst_list, 1, res)
s.check(res, all_list, xst_list, 1)
print(f'Program Executing Time: {time.time()-start}s')

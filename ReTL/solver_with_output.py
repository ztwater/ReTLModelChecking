import numpy as np
from sympy import *
from scipy.optimize import linprog
import sys

PINF = float("inf");
NINF = float("-inf");

#
# logic operations for inequalities
#
def AND(e1, e2):
    '''
    logicAnd for inequalities
    
    @param e1, e2: inequalities in the form of [np.array([[]), ...]
    @return e: a new inequality system when both e1 and e2 are satisfied
    '''
    # Deal with true/false value
    if isinstance(e1,int):
        if e1 == 0:
            return 0
        elif e1 == 1:
            return e2
    if isinstance(e2,int):
        if e2 == 0:
            return 0
        elif e2 == 1:
            return e1
    # Combine each possible inequality matrix in two systems
    e = []
    if len(e1) == 0:
        return e2
    if len(e2) == 0:
        return e1
    for a in e1:
        for b in e2:
            e.append(np.r_[a,b])
    return e

def OR(e1, e2):
    '''
    logicOr for inequalities
    
    @param e1, e2: inequalities in the form of [np.array([[]), ...]
    @return e: a new inequality system when either e1 or e2 is satisfied
    '''
    if isinstance(e1,int):
        if e1 == 1:
            return 1
        elif e1 == 0:
            return e2
    if isinstance(e2,int):
        if e2 == 1:
            return 1
        elif e2 == 0:
            return e1
        
    # connect two lists together
    e = e1 + e2
    return e
    
def NOT(e1):
    '''
    logicNot for inequalities
    
    @param e1: inequalities in the form of [np.array([[]), ...]
    @return e: a new inequality system when e1 is unsatisfied
    '''
    if isinstance(e1,int):
        return 1 - e1
    
    e = []
    e1_ = [np.array(x, dtype=object) for x in e1]
    for element in e1_[0]:
        e.append(np.array([Neg(element)], dtype=object))
    for ele in e1_[1:]:
        tmp = []
        for element in ele:
            tmp.append(np.array([Neg(element)], dtype=object))
        e = AND(e, tmp)
    return e
        
def Neg(element):
    if (element[1] == 'L'):
        element[1] = '>'
    elif (element[1] == '<'):
        element[1] = 'G'
    elif (element[1] == '='):
        element[1] = 'N'
    elif (element[1] == 'N'):
        element[1] = '='
    elif (element[1] == '>'):
        element[1] = 'L'
    elif (element[1] == 'G'):
        element[1] = '<'
    return element

def circ(e, n, l, M, b):
    layer_num = len(l)
    if n in l[-1]:
        print('Layers overflow!')
        return
    else:
        if n == 0:
            if layer_num > 2:
                return relu(np.dot(M[0],e) + b[0])
            else:
                return list(np.dot(M[0],e) + b[0])  
        i = 0
        for layer in l:
            if n in layer:
                break
            i += 1
        if i < layer_num - 2:
            return relu(np.dot(M[i],e) + b[i])
        elif i == layer_num - 2:
            return list(np.dot(M[i],e) + b[i])
        else:
            print('Layer Error:', e, n)
            return []

def get_dim(n, d, l):
    layer_num = len(l)
    i = 0
    for layer in l:
        if n in layer:
            break
        i += 1
    if i >= layer_num:
        print('Layer Error:', n)
    else:
        return 2 ** d[i]

def vec_dot(v1, v2):
    tmp = np.dot(np.array(v1, dtype=object), np.array(v2, dtype=object))
    return tmp

def vec_add(v1, v2):
    tmp = np.array(v1, dtype=object) + np.array(v2, dtype=object)
    return list(tmp)

def vec_sub(v1, v2):
    tmp = np.array(v1, dtype=object) - np.array(v2, dtype=object)
    return list(tmp)

def relu(v):
    '''
    do ReLU-elimination by adding constraints
    '''
    eye = np.eye(len(v))
    constraints = []
    res = []
    for i in range(2**len(v)):
        n = i
        c = len(v)-1
        E_U = np.zeros((len(v), len(v)))
        while n > 0:
            E_U[c][c] = n & 1
            c -= 1
            n = n >> 1
        a = np.dot(np.r_[E_U, E_U-eye], v) 
        e = ['G'] * 2 * len(v)
        b = [0] * 2 * len(v)
        constraints += get_inequalities(a, e, b)
        res.append(a[0:len(v)])
    return res, constraints
 
def Next(e):
    return e

def Until(e1, e2):
    return 1

def Exists(v, e):
    return e

def Forall(v, e):
    return e

def get_inequalities(a, e, b):
    '''
    return a list of inequalities
    '''
    l = len(e)
    if(l != len(a) & l != len(b)):
        print('Dimensions not match!\n')
        return False;
    mat = []
    for i in range(l):
        mat.append([a[i], e[i], b[i]])
    return [np.array(mat, dtype=object)]
    
def get_coeff(var_list, left):
    '''
    simplify inequalities and get coefficients according to variable list
    '''
    tmp = [0]*len(var_list)
    if isinstance(left, int) or isinstance(left, float):
        return tmp, -left
    k = 0
    for var in var_list:
        left, c = left.as_independent(var, as_Add=True)
        tmp[k] = c / var
        k += 1
    return tmp, -left

def unify(e, h):
    if h == 1:
        e = NOT(e)
        return e
    return e

def Intersect(a, b):
    '''
    Intersection of feasible regions
    
    @param a, b: lists of feasible regions(union)
    @return r: intersection of two regions
    '''
    if a == [] or b == []:
        return []
    r = []
    for ia in a:
        for ib in b:
            if len(ia) != len(ib):
                print(a, b)
                print("Unexpected error during intersecting")
                sys.exit()
            tmp = []
            for j in range(len(ia)):
                tmp.append(ia[j].intersect(ib[j]))
            if EmptySet not in tmp and tmp not in r:
                r.append(tmp)
    return r

def q_solve(e, var_list, all_list, xst_list, h, res):
    '''
    solve a PE problem with qualifrers
    
    @param
        res: previous results
        e: inequalities
        var_list: all variables in formula
        all_list: variables with qualifier forall
        xst_list: variables with qualifier exists
        h: hierarchy of qualifier-nesting
            1: inner layer is forall
            -1: inner layer is exists
    @return
        res: current results

    '''

    # qualifier free formulas
    if h == 0:
        if e == 1:
            print('Checking complete: True')
            print('It is a trivial case.')
            sys.exit()
        elif e == 0:
            print('Result is not certain.')
            return []
        else:
            # no constraints
            if e == []:
                print('Checking complete: True')
                print('It is a trivial case.')
                sys.exit()
            for ele in e:
                if solve(ele, [], 0) == True:
                    print('Checking complete: True')
                    print('It is a trivial case.')
                    sys.exit()
            print('Result is not certain.')
            return []
        
    elif h == 1 or h == -1:
        # # Sigma_1 or Pi_1 Formula
        if len(all_list) == 0:
            if h == -1:
                print('Start checking formula of the form <exists x. p>')
            else:
                print('Start checking formula of the form <forall x. p>')
            for ele in e:
                print("Checking linear inequalities:\n", ele)
                try:
                    res_min, res_max, interval = solve(ele, var_list, 0)
                except ValueError:
                    if 'infeasible' in str(sys.exc_info()[1]):
                        continue
                    else:
                        raise ValueError(sys.exc_info()[1])
                # content of solutions 
                # print(res_min)
                # print(res_max)
                
                # if there is a solution for LIs
                if res_min.status != 2 and res_max.status != 2:
                    print(f'Interval of {var_list[0]} is {interval}')
                    tmp = ele[:]
                    for i in range(len(var_list)-1):
                        res_p = pick(Interval(res_min.fun, -res_max.fun)) # get possible value of last var (average)
                        tmp = np.r_[tmp, [[var_list[i], '=', res_p]]] # add the equation as an constraint to LIs
                        res.append(res_p)
                        print(var_list[i], '=', res_p)
                        res_min, res_max, interval = solve(tmp, var_list, i+1) # solve next var
                        print(f'Interval of {var_list[i+1]} is {interval}')
                    res_p = pick(Interval(res_min.fun, -res_max.fun))
                    res.append(res_p)
                    print(var_list[-1], '=', res_p)
                    if h == -1:
                        print('Checking complete: True')
                        print(f'Examples for {var_list} is {res}.')
                    elif h == 1:
                        print('Checking complete: False')
                        print(f'Counter-examples for {var_list} is {res}.')
                    sys.exit()
            print('Result is not certain.')
            # print('Checking complete: False')
            # print(f'No suitable {xst_list} for this formula.')
            return []
        # Sigma_2 or Pi_2 Formula
        else:
            if h == -1:
                print('Start checking formula of the form <forall y. exists x. p>')
            else:
                print('Start checking formula of the form <exists y. forall x. p>')
            # tmp = [] # intervals of y
            for ele in e:
                y = []
                i = 0 # i-th variable in var_list
                print("Checking linear inequalities:\n", ele)
                for v in var_list:
                    y_v = [Reals] * len(all_list)
                    if v in all_list:
                        try:
                            res_min, res_max, interval = solve(ele, var_list, i)
                        except ValueError:
                            # 
                            if 'infeasible' in str(sys.exc_info()[1]):
                                y.append(y_v)
                                i += 1
                                continue
                            else:
                                raise ValueError(sys.exc_info()[1])
                                
                        # content of solutions 
                        # print(res_min)
                        # print(res_max)
                        
                        # unbounded results of interval
                        if res_min.status == 3:
                            interval = Interval(-oo, interval.sup)
                        if res_max.status == 3:
                            interval = Interval(interval.inf, +oo)
                        
                        print(f'Interval of {v} is {interval}')
                            
                        # complement not empty
                        if interval != Reals:
                            if res_min.status != 2 and res_max.status != 2:
                                y_v[all_list.index(v)] = interval.complement(Reals)
                            if y_v not in y:
                                y.append(y_v)
                    i += 1
                print(y)
                print(res)
                res = Intersect(res, y)
                print(res)
            return res
    else:
        print("Not support this kind of formula.")
        sys.exit()

def solve(e, var_list, i):
    '''
    solve linear inequalities with LP
    
    @param
        e: linear inequalities
        var_list: variables exists in formulas
        i: i-th position of variables
    
    @return
        res_min: solution for i-th variable's minvalue
        res_max: solution for i-th variable's maxvalue
        interval: i-th variables solution
    '''
    if len(var_list) == 0:
        for element in e:
            if element[1] == '=':
                if element[0] != element[2]:
                    return 0
            if element[1] == '<':
                if element[0] >= element[2]:
                    return 0
            if element[1] == 'L':
                if element[0] > element[2]:
                    return 0
            if element[1] == '>':
                if element[0] <= element[2]:
                    return 0
            if element[1] == 'G':
                if element[0] < element[2]:
                    return 0
            if element[1] == 'N':
                if element[0] == element[2]:
                    return 0
        return 1
            
    coeff_eq = []
    coeff_in = []
    const_eq = []
    const_in = []
    # used for transfer conditions with strict signs e.g. x < 1 -> x <= 1 - epsilon
    epsilon = 0.000001
    for element in e:
        if element[1] == '=':
            e1, e2 = get_coeff(var_list, element[0]-element[2])
            coeff_eq.append(e1)
            const_eq.append(e2)
        if element[1] == '<':
            e1, e2 = get_coeff(var_list, element[0]-element[2]+epsilon)
            coeff_in.append(e1)
            const_in.append(e2)
        if element[1] == 'L':
            e1, e2 = get_coeff(var_list, element[0]-element[2])
            coeff_in.append(e1)
            const_in.append(e2)
        if element[1] == '>':
            e1, e2 = get_coeff(var_list, element[2]-element[0]+epsilon)
            coeff_in.append(e1)
            const_in.append(e2)
        if element[1] == 'G':
            e1, e2 = get_coeff(var_list, element[2]-element[0])
            coeff_in.append(e1)
            const_in.append(e2)
        if element[1] == 'N':
            e1, e2 = get_coeff(var_list, element[0]-element[2]+epsilon)
            coeff_in.append(e1)
            const_in.append(e2)
            e1, e2 = get_coeff(var_list, element[2]-element[0]+epsilon)
            coeff_in.append(e1)
            const_in.append(e2)
    # if matrix is empty, assign zero to its values
    if len(coeff_eq) == 0:
        coeff_eq = [[0]*len(var_list)]
    if len(coeff_in) == 0:
        coeff_in = [[0]*len(var_list)]
    if len(const_eq) == 0:
        const_eq = [0]
    if len(const_in) == 0:
        const_in = [0]
    # print(coeff_eq, coeff_in, const_eq, const_in)
    sign_mat = np.eye(len(var_list))
    coeff_min = sign_mat[i]
    res_min = linprog(coeff_min,
                  A_ub=coeff_in,
                  b_ub=const_in,
                  A_eq=coeff_eq,
                  b_eq=const_eq,
                  bounds = (NINF, PINF))

    coeff_max = -sign_mat[i]
    res_max = linprog(coeff_max,
                  A_ub=coeff_in,
                  b_ub=const_in,
                  A_eq=coeff_eq,
                  b_eq=const_eq,
                  bounds = (NINF, PINF))
    interval = Interval(round(res_min.fun,5),round(-res_max.fun,5))
    return res_min, res_max, interval

def pick(I):
    if isinstance(I, Interval):
        if I.inf == -oo and I.sup == +oo:
            return 0
        elif I.inf == -oo:
            return I.sup
        else:
            return I.inf
    if isinstance(I, Set):
        return I.inf
    print("Type error");

def check(res, all_list, xst_list, h):
    if res == []:
        if h == 1:
            if all_list == []:
                print('Checking complete: True')
                print(f'All values of {xst_list} satisfy the formula.')
            else:
                print('Checking complete: False')
                print(f'No feasible solutions for {all_list}.')
        elif h == -1:
            if all_list == []:
                print('Checking complete: False')
                print(f'No feasible solutions for {xst_list}.')
            else:
                print('Checking complete: True')
                print(f'All values of {all_list} satisfy the formula.')
        # qualifier-free formula
        else:
            print('Checking complete: False')
            print('It is a trivial case.')
    else:
        tmp = [pick(yi) for yi in res[0]];    
        if h == -1:
            print('Checking complete: False')
            print(f'Counter-example for {all_list} is {tmp}.')
        else:
            print('Checking complete: True')
            print(f'Witness for {all_list} is {tmp}.')
    return
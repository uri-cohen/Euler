#!/usr/bin/env python3

"""
Investigating progressive numbers, n, which are also square
Problem 141
A positive integer, n, is divided by d and the quotient and remainder are q and r respectively.
In addition d, q, and r are consecutive positive integer terms in a geometric sequence, 
but not necessarily in that order.

For example, 58 divided by 6 has quotient 9 and remainder 4. It can also be seen 
that 4, 6, 9 are consecutive terms in a geometric sequence (common ratio 3/2).
We will call such numbers, n, progressive.

Some progressive numbers, such as 9 and 10404 = 102^2, happen to also be perfect squares.
The sum of all progressive perfect squares below one hundred thousand is 124657.

Find the sum of all progressive perfect squares below one trillion (10^12).
"""

import sympy
import math

def sub_exponent_tuples(t):
    (i, l) = (0, len(t))
    s = [-1] + (l-1)*[0]
    while i < l:
        s[i] += 1
        if s[i] <= t[i]:
            i = 0
            yield(tuple(s))
            continue
        s[i] = 0
        i += 1

def complement_exp(t, s):
    return tuple([et-es for (et, es) in zip(t, s)])

def base_exp(n):
    h = sympy.factorint(n)
    b = tuple([i for i in h])
    e = tuple([h[i] for i in b])
    return (b, e)

def defactor(b, e):
    r = 1
    for (f, p) in zip(b, e): r *= (f**p)
    return r

def geometric_seqs(n):
    (b, e) = base_exp(n*n)
    for e0 in sub_exponent_tuples(e):
        e1 = complement_exp(e, e0)
        (m, o) = (defactor(b, e0), defactor(b, e1))
        if m < n: yield (m,n,o)

def is_square(n):
    return int(math.sqrt(n))**2 == n

def progressive_squares(n):
    ps = set()
    n2 = int(math.sqrt(n)) + 1
    print(n2)
    for i in range(n2):
        if i%100000 == 0: print(i)
        for (a,b,c) in geometric_seqs(i):
            for s in [c*b+a, c*a+b]:
                if s<=n and is_square(s):
                    ps.add(s)
    return sum(ps)

print(progressive_squares(10**12))

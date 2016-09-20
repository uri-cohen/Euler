#!/usr/bin/env python3

import sympy

def Mmks(m,k,s,n):
    if n>m: return n-s
    return Mmks(m,k,s, Mmks(m,k,s,n+k))

def S(p,m):
    sum = 0
    for k in range(1,p+1):
        for ds in sympy.divisors(k)[:-1]:
            s = k - ds
            first = m - s + 1
            last = min(m, m - s + ds)
            sum += (last-first+1)*(first+last)/2
    return sum

for (p,m) in [(10,10), (1000,1000), (1000000, 1000000)]:
    print("S(%d,%d) = %Ld" % (p,m,S(p,m)))

#!/usr/bin/env python3

import sympy

class AbcHit:
    """
abc-hits
Problem 127
The radical of n, rad(n), is the product of distinct prime factors of n. For example, 504 = 2^3 × 3^2 × 7, so rad(504) = 2 × 3 × 7 = 42.

We shall define the triplet of positive integers (a, b, c) to be an abc-hit if:

1. GCD(a, b) = GCD(a, c) = GCD(b, c) = 1
2. a < b
3. a + b = c
4. rad(abc) < c

For example, (5, 27, 32) is an abc-hit, because:
1. GCD(5, 27) = GCD(5, 32) = GCD(27, 32) = 1
2. 5 < 27
3. 5 + 27 = 32
4. rad(4320) = 30 < 32

It turns out that abc-hits are quite rare and there are only thirty-one abc-hits for c < 1000, with sum(c) = 12523.

Find sum(c) for c < 120000.
"""

    def __init__(self, n):
        self.n = n
        self.sum_c = 0
        self.cs = set()
        self.abc = []
        self.init_factorint_rad()
        print("end of init")
        self.solve()
        self.print()

    def init_factorint_rad(self):
        fact = self.factorint = [{}, {}]
        rad = self.rad = [0, 1]

        for i in range(2,self.n):
            f = sympy.factorint(i)
            fact.append(f)
            r = 1
            for j in f.keys(): r *= j
            rad.append(r)

    def solve(self):
        for a in range(1, int(self.n/2)):
            self.solve_a(a)

    def solve_a(self, a):
        if (a%5000) == 0: print("...", a)
        bs = self.b_candidates(a)
        for b in bs:
            c = a+b
            #if c in self.cs:
            #    continue
            rad_abc = self.rad[a] * self.rad[b] * self.rad[c]
            if rad_abc < c:
                self.sum_c += c
                self.cs.add(c)
                self.abc.append((a,b,c))

    def b_candidates(self, a):
        ul = self.n - a
        candidates = set(range(max(a+1, self.rad[a]*5), ul))
        for f in self.factorint[a].keys():
            candidates = candidates - set(range(a, ul, f))
        return candidates

    def print(self):
        print("abc-hit(", self.n, "):")
        print("possible c values =")
        for (a,b,c) in self.abc:
            print(a,b,c, self.rad[a] * self.rad[b] * self.rad[c])
        print("sum(c | c < ", self.n, ") = ", self.sum_c)



# AbcHit(1000)
AbcHit(120000)

#!/usr/bin/env python3

"""
Let a0, a1, a2, ... be an integer sequence defined by:

a0 = 1;
for n ≥ 1, an is the sum of the digits of all preceding terms.
The sequence starts with 1, 1, 2, 4, 8, 16, 23, 28, 38, 49, ...
You are given a[10^6] = 31054319.

Find a[10^15].
"""

def digit_sum(n):
    sum = 0
    while n>0:
        sum += n%10
        n = int(n/10)
    return sum

a = 1
a0 = 1
print(0,1)
print(1,1)
for i in range(2,1000000):
    ds = digit_sum(a)
    a += ds
    print(i, a)

print(a)

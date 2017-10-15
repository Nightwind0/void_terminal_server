#!/usr/bin/python

import numpy
import math
import random

def calc_spread(start):
    spread = start
    # So, if we keep geting lucky, we can get a bigger and bigger spread. For a chance of 10%, one out of 100 times we will get a much bigger spread
    while True:
        if random.random() < 0.10:
            spread = spread * 10
        else:
            return spread
max_spread = 0
for n in range(1,20000):
    #power = max(1,int(math.log(n,10))-2);
    #spread = pow(10,power);
    spread = calc_spread(10);
    if spread > max_spread:
        max_spread = spread
    print "[%d] %d -> %d, %d, %d, %d" %(spread, n, int(numpy.random.normal(n, spread)), int(numpy.random.normal(n,spread)), int(numpy.random.normal(n,spread)), int(numpy.random.normal(n,spread)))

print max_spread

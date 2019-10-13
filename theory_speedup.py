#!/usr/bin/env python
import numpy as np
def S(p, D=8000):
    # p = p-1
    rs = np.mod(D**2, p-1) / D**2
    rp = 1 - rs
    sp = 1/(rs + rp/p)
    print("speedup in theory:\np = {0}\nrs = {1}\nrp = {2}\nsp = {3}\n".format(p, rs, rp, sp))
    return sp
if __name__ == "__main__":
    S(np.array([2,4,6,8,10]))

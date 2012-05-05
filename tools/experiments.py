#!/usr/bin/env python2
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from argparse import ArgumentParser
from subprocess import check_output
import math
import os
import sys

import numpy

class Sfn(object):
    def __init__(self, path):
        self.path = path
        self.devnull = open(os.devnull, 'w')

    def __call__(self, n0, m, t, seed=4, error=0.01):
        k = math.ceil( math.log(2 * (n0 + t)) / (2 * error**2) )
        args = (self.path, '-n %d' % (n0,), ('-m %d' % (m,)), ('-T %d' % (t,)),
                '-s %d' % (seed,), '-k %d' % (k))

        output = check_output(args, stderr=self.devnull)

        res = [n0, m, t, error, k, seed]
        for line in output.strip().split('\n'):
            tokens = line.split()
            res.append(float(tokens[1]))
        return res

def results(sfn, n0=10, m=10, t=20000, step=100):
    res = []
    for t_ in xrange(1, t + step, step):
        r = sfn(n0, m, t_)
        print('\t'.join('%g' % i for i in r))
        res.append(r)
    return numpy.array(res)

def build_argument_parser():
    ap = ArgumentParser()
    add = ap.add_argument
    add('-t', default=3000, type=int)
    add('sfn')
    add('res')
    return ap

def main(argv=None):
    if argv is None:
        argv = sys.argv
    args = build_argument_parser().parse_args(args=argv[1:])
    sfn = Sfn(args.sfn)
    res = results(sfn, t=args.t)
    with open(args.res, 'wb') as resfile:
        numpy.save(resfile, res)
    return 0

if __name__ == '__main__':
    exit(main())


#!/usr/bin/env python2
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from argparse import ArgumentParser
from subprocess import check_output
from time import time
import sys

import matplotlib.pyplot as plt

import numpy

class Sfn(object):
    def __init__(self, path):
        self.path = path

    def __call__(self, n0, m, t):
        args = (self.path, '-n %d' % (n0,), ('-m %d' % (m,)), ('-T %d' % (t,)))
        before = time()
        output = check_output(args)
        after = time()
        res = [n0, m, t, after - before]
        for line in output.strip().split('\n'):
            tokens = line.split()
            res.append(float(tokens[1]))
        return res


def results(sfn, n0=10, m=10, t=20000, step=1000):
    return numpy.array([sfn(n0, m, t_) for t_ in xrange(1, t + step, step)])


def plot(res):
    plt.scatter(res[:,2], res[:,4])
    plt.scatter
    plt.show()

def build_argument_parser():
    ap = ArgumentParser()
    add = ap.add_argument
    add('-l', '--load', action='store_true', default=False)
    add('sfn')
    add('res')
    return ap

def main(argv=None):
    if argv is None:
        argv = sys.argv
    args = build_argument_parser().parse_args(args=argv[1:])
    if args.load:
        print("Loading cached results.")
        with open(args.res, 'rb') as resfile:
            res = numpy.load(resfile)
    else:
        print("Calculating results.")
        sfn = Sfn(args.sfn)
        res = results(sfn)
    plot(res)
    if not args.load:
        with open(args.res, 'wb') as resfile:
            numpy.save(resfile, res)
        print("Saved results.")
    return 0

if __name__ == '__main__':
    exit(main())


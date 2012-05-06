#!/usr/bin/env python2
from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from argparse import ArgumentParser
import sys

from matplotlib import rc
from matplotlib import pyplot as plt

import numpy

rc('text', usetex=True)
rc('font', family='serif')

# 0 - n0
# 1 - m
# 2 - t
# 3 - error
# 4 - k
# 5 - seed
# 6 - approx. cc
# 7 - approx. runtime
# 8 - cc
# 9 - runtime

def plot_cc(r, output):
    plt.clf()
    plt.semilogy(r[:,2] + r[0, 0], r[:,8], 'b-', label='Exact')
    plt.semilogy(r[:,2] + r[0, 0], r[:,6], 'r-', label='Approx.')
    plt.autoscale(tight=True)
    plt.legend()
    plt.title(r'Network size versus Clustering coefficient '
              r'($n_0 = %d ,\; m = %d ,\; \epsilon = %3g$).'
              % (r[0,0], r[0,1], r[0,3]))
    plt.xlabel('Nodes')
    plt.ylabel('Clustering coefficient')
    if output == '-':
        plt.show()
    else:
        plt.savefig(output)

def plot_time(r, output):
    plt.clf()
    plt.semilogy(r[:,2] + r[0, 0], r[:,9], 'b-', label='Exact')
    plt.semilogy(r[:,2] + r[0, 0], r[:,7], 'r-', label='Approx.')
    plt.autoscale(tight=True)
    plt.legend(loc='upper left')
    plt.title(r'Network size versus execution time '
              r'($n_0 = %d ,\; m = %d ,\; \epsilon = %3g$).'
              % (r[0,0], r[0,1], r[0,3]))
    plt.xlabel('Nodes')
    plt.ylabel('Execution time (ms)')
    plt.ylim(1)
    if output == '-':
        plt.show()
    else:
        plt.savefig(output)

def build_argument_parser():
    ap = ArgumentParser()
    add = ap.add_argument
    add('-c', '--plot-cc', const='-', nargs='?')
    add('-t', '--plot-time', const='-', nargs='?')
    add('results')
    return ap

def main(argv=None):
    if argv is None:
        argv = sys.argv
    args = build_argument_parser().parse_args(args=argv[1:])
    with open(args.results, 'rb') as results_file:
        results = numpy.load(results_file)
    if args.plot_cc:
        plot_cc(results, output=args.plot_cc)
    if args.plot_time:
        plot_time(results, output=args.plot_time)
    return 0

if __name__ == '__main__':
    exit(main())


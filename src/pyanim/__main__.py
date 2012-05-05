from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from argparse import ArgumentParser
import sys

from pyanim import Frames

def build_argument_parser():
    ap = ArgumentParser()
    add = ap.add_argument
    add('animation')
    add('outdir')
    return ap

def main(argv=None):
    if argv is None:
        argv = sys.argv
    args = build_argument_parser().parse_args(args=argv[1:])
    frames = Frames.from_file(args.animation)
    frames(args.outdir)
    return 0

if __name__ == '__main__':
    exit(main())


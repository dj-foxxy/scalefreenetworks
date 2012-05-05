#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${REPO}"

make build

python2 tools/plots.py results/results.npy -c -t &
python2 tools/plots.py -c bin/cc.eps -t bin/time.eps results/results.npy


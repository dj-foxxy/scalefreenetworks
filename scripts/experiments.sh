#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${REPO}"

make build

python2 ./tools/experiments.py -t 20000 bin/sfn/sfn results/results.npy


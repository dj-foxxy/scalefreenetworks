#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${REPO}"

make build

anim=/tmp/animation
rm -fr "${anim}"
mkdir "${anim}"
./bin/sfn/sfn -n 2 -T 10 -m 2 -a "${anim}/animation"
cat "${anim}/animation"
PYTHONPATH=src/ python2 -m pyanim "${anim}/animation" "${anim}"
eog ${anim}/0.png


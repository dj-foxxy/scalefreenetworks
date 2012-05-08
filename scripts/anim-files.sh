#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${REPO}"

make build

anim=bin/animation
mkdir -p "${anim}"
python tools/tsv_to_edges.py datasets/marvel_social_2.tsv > "${anim}/edges"
./bin/sfn/sfn -R -a "/tmp/real.anim" -e "${anim}/edges"
head -n +10000 /tmp/real.anim > "${anim}/real.anim"
./bin/sfn/sfn -A -a "/tmp/approx.anim" -e "${anim}/edges"
head -n +1000 /tmp/approx.anim > "${anim}/approx.anim"


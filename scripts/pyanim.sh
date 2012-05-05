#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"

PYTHONPATH="${REPO}/src/" python2 -m pyanim "${@}"


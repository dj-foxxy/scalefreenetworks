#!/bin/bash

set -o errexit
set -o nounset

readonly REPO="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${REPO}/.."

rm -f group2.gzip
tar -zvcf group2.gzip --exclude='.git' "${REPO}"

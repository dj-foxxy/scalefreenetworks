#!/bin/bash

set -o errexit
set -o nounset

readonly PROJ="$(readlink -f -- "$(dirname -- "${0}")/..")"
cd -- "${PROJ}"

make build

./bin/sfn -d /tmp/sfn.dot "${@}"
dot -T png -o /tmp/sfn.png /tmp/sfn.dot
eog /tmp/sfn.png

